#ifndef __RS_FFMPEG_DECODER_TASK_H__
#define __RS_FFMPEG_DECODER_TASK_H__

#include <vector>
#include "hw/hlog.h"
#include "rs_ffmpeg_util.h"
#include "libutils/hmedia.h"
#include "rs_avframe_cxx.h"
#include "rs_date.h"
#include "rs_chained_io_if.h"

#define HARDWARE_DECODE     1
#define SOFTWARE_DECODE     2

#define DEFAULT_DECODE_MODE HARDWARE_DECODE

class RSFFMpegDecoderTask : 
	public RSChainedIOTask<spRSAVEvent>
{
public:
	RSFFMpegDecoderTask();
	~RSFFMpegDecoderTask();
	int		GetFps();
	void	SetMedia(HMedia hMedia);

protected:
	virtual void Run();

	int		Init();
	int		Decode();
	int		GetCodec();

private:
	void	CleanUp();
	int		GetVideoSource();
	//void	makeFrame();
	int     hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type);
	static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
    	                                    const enum AVPixelFormat *pix_fmts);
	enum StreamType {
		STREAM_TYPE_NONE = 0,
		STREAM_TYPE_FILE,
		STREAM_TYPE_NET,
		STREAM_TYPE_MAX
	};
	enum StreamType GetStreamType();
	int64_t GetRetryDelayMS() {
		if (retryCnt < 8)
			retryCnt++;
		//retryCnt = retryCnt % 9;
		const int64_t delayBase = 100; // 100ms
		return delayBase << retryCnt;  // max 25.600 s
	}
	void ResetRetryCnt() {
		retryCnt = 0;
	}
protected:
	HFrame		hFrame_;
	//readsense::RSFrame<uint8> rsFrame_;

	HMedia		hMedia_;

	AVInputFormat		*pInputFormat_ = NULL;
	AVFormatContext		*pAVFormatCtx_ = NULL;
	AVCodecContext		*pAVCodecCtx_ = NULL;
	AVFrame				*pAVFrame_ = NULL;
	AVPacket			*pPacket_ = NULL;
	AVPixelFormat		dstPixelFormat_;
	SwsContext			*pSwsCtx_ = NULL;
	AVBufferRef         *hw_device_ctx = NULL;
	// TODO: hw_pix_fmt should be an array, since multiple format might be supported
	static enum AVPixelFormat  hw_pix_fmt;

	std::string	videoSrc_;
	int         decode_mode = HARDWARE_DECODE;

	int video_stream_index;
	int audio_stream_index;
	int subtitle_stream_index;

	AVRational  video_time_base;
	int         fps = VIDEO_FALL_BACK_FPS;
	int64       duration = 0;	// ms
	int64       start_time = 0; // ms
	int         signal;

	uint8	*pData_[4];
	int		lineSize_[4];

	int retryCnt = 0;
};

enum AVPixelFormat  RSFFMpegDecoderTask::hw_pix_fmt = AV_PIX_FMT_NONE;

RSFFMpegDecoderTask::RSFFMpegDecoderTask()
{
	pInputFormat_ = NULL;
	pAVFormatCtx_ = NULL;
	pAVCodecCtx_ = NULL;
	pAVFrame_ = NULL;
	pPacket_ = NULL;
	pSwsCtx_ = NULL;
}

RSFFMpegDecoderTask::~RSFFMpegDecoderTask()
{
	CleanUp();
}

int RSFFMpegDecoderTask::Init()
{
	av_register_all();
	avcodec_register_all();

	if (GetStreamType() == STREAM_TYPE_NONE)
		return -1;

	return 0;
}

void RSFFMpegDecoderTask::CleanUp()
{
	pInputFormat_ = NULL;
	pAVFormatCtx_ = NULL;
	pAVCodecCtx_ = NULL;
	pAVFrame_ = NULL;
	pPacket_ = NULL;
	pSwsCtx_ = NULL;

	if (pAVCodecCtx_) {
		avcodec_close(pAVCodecCtx_);
		avcodec_free_context(&pAVCodecCtx_);
		pAVCodecCtx_ = NULL;
	}

	if (pAVFormatCtx_) {
		avformat_close_input(&pAVFormatCtx_);
	}

	if (pAVFormatCtx_) {
		avformat_free_context(pAVFormatCtx_);
		pAVFormatCtx_ = NULL;
	}

	if (pAVFrame_) {
		av_frame_unref(pAVFrame_);
		av_frame_free(&pAVFrame_);
		pAVFrame_ = NULL;
	}

	if (pPacket_) {
		av_packet_unref(pPacket_);
		av_packet_free(&pPacket_);
		pPacket_ = NULL;
	}

	if (pSwsCtx_) {
		sws_freeContext(pSwsCtx_);
		pSwsCtx_ = NULL;
	}
}

void RSFFMpegDecoderTask::SetMedia(HMedia hMedia)
{
	hMedia_ = hMedia;
}

enum RSFFMpegDecoderTask::StreamType RSFFMpegDecoderTask::GetStreamType() {
	std::string uri = hMedia_.src;
	if (!uri.length())
		return STREAM_TYPE_NONE;
	std::size_t sep_pos = uri.find_first_of("/");
	if (sep_pos == 0 || sep_pos == std::string::npos || uri.find_first_of("file://") == 0)
		return STREAM_TYPE_FILE;
	sep_pos = uri.find_first_of(":/");
	std::string schema = uri.substr(0, sep_pos);
	static const std::vector<std::string> supportedSchema {
		"rtsp", "rtmp",
		"http", "https",
		"udp", "tcp"
	};

	for (auto sch : supportedSchema) {
		if (schema == sch)
			return STREAM_TYPE_NET;
	}
	return STREAM_TYPE_FILE;
}

int RSFFMpegDecoderTask::hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
                                      NULL, NULL, 0)) < 0) {
        fprintf(stderr, "Failed to create specified HW device.\n");
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    return err;
}

#warning hw_pix_fmt shared by thread, need lock?
enum AVPixelFormat RSFFMpegDecoderTask::get_hw_format(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts)
{
    const enum AVPixelFormat *p;
	fprintf(stderr, "enter get_hw_format\n");
    for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
		fprintf(stderr, "HW surface format: %d %s\n", *p, av_get_pix_fmt_name(*p));
        if (*p == hw_pix_fmt)
            return *p;
    }

    fprintf(stderr, "Failed to get HW surface format.\n");
	exit(1);
    return AV_PIX_FMT_NONE;
}

int RSFFMpegDecoderTask::GetCodec()
{
	int iRet = 0;
	enum AVHWDeviceType type = AV_HWDEVICE_TYPE_VAAPI;
	/* AVHWDeviceType
		Intel: AV_HWDEVICE_TYPE_VAAPI
		Nvidia: AV_HWDEVICE_TYPE_CUDA
		Mac: AV_HWDEVICE_TYPE_VIDEOTOOLBOX
		Software: AV_HWDEVICE_TYPE_NONE
	*/

	GetVideoSource();

	pAVFormatCtx_ = avformat_alloc_context();
	if (pAVFormatCtx_ == NULL) {
		hloge("avformat_alloc_context");
		return -10;
	}
	iRet = avformat_open_input(&pAVFormatCtx_, videoSrc_.c_str(), pInputFormat_, NULL);
	if (iRet != 0) {
		hloge("Open input file[%s] failed: %d", videoSrc_.c_str(), iRet);
		return iRet;
	}

	iRet = avformat_find_stream_info(pAVFormatCtx_, NULL);
	if (iRet != 0) {
		hloge("Can not find stream: %d", iRet);
		return iRet;
	}
	hlogi("stream_num=%d", pAVFormatCtx_->nb_streams);

	AVCodec* codec = NULL;
	video_stream_index = av_find_best_stream(pAVFormatCtx_, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);

	if (video_stream_index < 0) {
		hloge("Can not find video stream.");
		return -20;
	}
	// av_dump_format(pAVFormatCtx_, video_stream_index, videoSrc_.c_str(), 0);

	if (type != AV_HWDEVICE_TYPE_NONE) {
		for (int i = 0;; i++) {
			const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);

			if (!config)
				break;
			if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
				config->device_type == type) {
				// use the first supported format
				if (hw_pix_fmt == AV_PIX_FMT_NONE)
					hw_pix_fmt = config->pix_fmt;
				printf("hw supported pixel format %d %s\n", config->pix_fmt, av_get_pix_fmt_name(config->pix_fmt));
				//break;
			}
		}
		if (hw_pix_fmt == AV_PIX_FMT_NONE) {
			fprintf(stderr, "Decoder %s does not support device type %s.\n",
					codec->name, av_hwdevice_get_type_name(type));
			// return -1;
			type = AV_HWDEVICE_TYPE_NONE;
		}
	}

	AVStream* video_stream = pAVFormatCtx_->streams[video_stream_index];
	video_time_base = video_stream->time_base;

	if (video_stream->avg_frame_rate.den && video_stream->avg_frame_rate.num)
		fps = av_q2d(video_stream->avg_frame_rate);
	if (video_stream->time_base.den && video_stream->time_base.num) {
		duration = video_stream->duration * av_q2d(video_stream->time_base) * 1000;
		start_time = video_stream->start_time * av_q2d(video_stream->time_base) * 1000;
	}
	hlogi("fps=%d duration=%lldms start_time=%lldms", fps, duration, start_time);

	AVCodecParameters* codecpar = video_stream->codecpar;
	hlogi("codec_id=%d:%s", codecpar->codec_id, avcodec_get_name(codecpar->codec_id));
	hlogi("codec_name: %s=>%s", codec->name, codec->long_name);
	
	pAVCodecCtx_ = avcodec_alloc_context3(codec);
	if (pAVCodecCtx_ == NULL) {
		hloge("avcodec_alloc_context3!");
		return -60;
	}

	iRet = avcodec_parameters_to_context(pAVCodecCtx_, codecpar);
	if (iRet != 0) {
		hloge("avcodec_parameters_to_context error: %d", iRet);
		return iRet;
	}


	if (type != AV_HWDEVICE_TYPE_NONE) {
		pAVCodecCtx_->get_format = &RSFFMpegDecoderTask::get_hw_format;
		if (hw_decoder_init(pAVCodecCtx_, type) < 0)
			return -1;
	}

	//iRet = avcodec_open2(pAVCodecCtx_, codec, NULL);
	iRet = avcodec_open2(pAVCodecCtx_, NULL, NULL);
	hlogi("avcodec_open2 ret: %d", iRet);
	if (iRet != 0) {
		hloge("Can not open codec: %d", iRet);
		return iRet;
	}

/* 	int sw = pAVCodecCtx_->width;
	int sh = pAVCodecCtx_->height;
	int dw = sw >> 2 << 2; // align = 4
	int dh = sh;

	makeFrame();

	pAVFrame_ = av_frame_alloc();
*/
	pPacket_ = av_packet_alloc();

	hlogi("src_pix_fmt=%d:%s dst_pix_fmt=%d:%s", \
		pAVCodecCtx_->pix_fmt, av_get_pix_fmt_name(pAVCodecCtx_->pix_fmt), \
		dstPixelFormat_, av_get_pix_fmt_name(dstPixelFormat_));
	return 0;
}

int RSFFMpegDecoderTask::Decode()
{
	int ret = -1;
	AVFrame *sw_frame = NULL;
    AVFrame *tmp_frame = NULL;

	if (!(pAVFrame_ = av_frame_alloc())){
		fprintf(stderr, "Can not alloc frame\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}

	while (!ShouldStop()) {
		//av_init_packet(pPacket_);
		ret = av_read_frame(pAVFormatCtx_, pPacket_);
		hlogi("av_read_frame, ret: 0x%x", ret);
		if (ret != 0) {
			if (GetStreamType() == STREAM_TYPE_FILE) {
				ret = 0;
				goto fail;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(GetRetryDelayMS()));
			continue;
		} else {
			ResetRetryCnt();
		}

		if (!pPacket_ || pPacket_->stream_index != video_stream_index) {
			av_packet_unref(pPacket_);
			continue;
		}

		if (avcodec_send_packet(pAVCodecCtx_, pPacket_) == 0) {
			// multiple frames for one packet
			while (avcodec_receive_frame(pAVCodecCtx_, pAVFrame_) == 0) {
				if (pAVFrame_->format == hw_pix_fmt) {
					if (!(sw_frame = av_frame_alloc())) {
						fprintf(stderr, "Can not alloc frame\n");
						ret = AVERROR(ENOMEM);
						goto fail;
					}
					/* retrieve data from GPU to CPU */
					if ((ret = av_hwframe_transfer_data(sw_frame, pAVFrame_, 0)) < 0) {
						fprintf(stderr, "Error transferring the data to system memory\n");
						goto fail;
					}
					tmp_frame = sw_frame;
					av_frame_unref(pAVFrame_);
				} else {
					tmp_frame = pAVFrame_;
				}

				if (tmp_frame->pts == AV_NOPTS_VALUE)
					tmp_frame->pts = pPacket_->pts;
				if (tmp_frame->pkt_dts == AV_NOPTS_VALUE)
					tmp_frame->pkt_dts = pPacket_->dts;
				//printf("push frame fmt %d size %dx%d\n", tmp_frame->format, tmp_frame->width, tmp_frame->height);
				RSAVFrame *avframe = new RSAVFrame;
				avframe->AllocFrame().MoveRefFrom(tmp_frame);
				avframe->info_.src_ = hMedia_.src;
				avframe->info_.fps_ = fps;
				avframe->info_.time_base = video_time_base;
				spRSAVEvent avevent = std::make_shared<spRSAVEvent::element_type>(avframe);

				Output(std::move(avevent));
			}
		}

		av_packet_unref(pPacket_);
	}

	ret = 0;
fail:
	return ret;
}

int RSFFMpegDecoderTask::GetVideoSource()
{
	switch (hMedia_.type) {
	case readsense::MEDIA_TYPE_CAPTURE:
	{
		videoSrc_ = "video=";
		videoSrc_ += hMedia_.src;
#ifdef _WIN32
		const char drive[] = "dshow";
#elif defined(__linux__)
		const char drive[] = "v4l2";
#else
		const char drive[] = "avfoundation";
#endif
		pInputFormat_ = av_find_input_format(drive);
		if (pInputFormat_ == NULL) {
			hloge("Can not find dshow");
			return -5;
		}
	}
	break;
	case readsense::MEDIA_TYPE_FILE:
	case readsense::MEDIA_TYPE_NETWORK:
		videoSrc_ = hMedia_.src;
		break;
	default:
		return -10;
	}

	return 0;
}
/*
void RSFFMpegDecoderTask::makeFrame() {
	int sw = pAVCodecCtx_->width;
	int sh = pAVCodecCtx_->height;

	int dw = sw >> 2 << 2; // align = 4
	int dh = sh;

	hFrame_.w = dw;
	hFrame_.h = dh;

	dstPixelFormat_ = AV_PIX_FMT_BGR24;
	hFrame_.type = 0x80E0;//GL_BGR;
	hFrame_.bpp = 24;
	hFrame_.buf.init(dw * dh * hFrame_.bpp / 8);
	rsFrame_.src_ = hMedia_.src;
	rsFrame_.width_ = dw;
	rsFrame_.height_ = dh;
	rsFrame_.type_ = 0x80E0;//GL_BGR;
	rsFrame_.bpp_ = 24;
	rsFrame_.init(dw * dh * 3);
	pData_[0] = rsFrame_.data();
	lineSize_[0] = dw * 3;
}
*/

int RSFFMpegDecoderTask::GetFps() {
	return fps;
}

void RSFFMpegDecoderTask::Run() {
	int ret = -1;
	if ((ret = Init()) != 0) {
		hloge("Init fail.\n");
		goto done;
	}
	if (!(ret = GetCodec())) {
		ret = Decode();
	} else {
		hloge("GetCodec failed.\n");
		goto done;
	}
done:
	FinishTaskChain(ret);
    OutputDateTime();
	std::cout << "RSFFMpegDecoderTask::Run exited." << std::endl;
	return;
}

#endif // __RS_FFMPEG_DECODER_TASK_H__
