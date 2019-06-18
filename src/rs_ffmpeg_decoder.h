#ifndef __RS_FFMPEG_DECODER_H__
#define __RS_FFMPEG_DECODER_H__

#include "hw/hlog.h"
#include "rs_ffmpeg_util.h"
#include "libutils/hmedia.h"
//#include "rs_frame.h"

#define HARDWARE_DECODE     1
#define SOFTWARE_DECODE     2

#define DEFAULT_DECODE_MODE HARDWARE_DECODE

class RSFFMpegDecoder {
public:
	RSFFMpegDecoder();
	~RSFFMpegDecoder();

public:
	void	Init();
	int		Decode();
	int		GetCodec();

	int		GetFps();
	void	SetMedia(HMedia hMedia);

private:
	void	cleanup();
	int		getVideoSource();
	void	makeFrame();
	int     hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type);
	static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
    	                                    const enum AVPixelFormat *pix_fmts);
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

	int video_stream_index;
	int audio_stream_index;
	int subtitle_stream_index;

	int video_time_base_num;
	int video_time_base_den;

	int         fps;
	int         decode_mode = HARDWARE_DECODE;
	int64       duration;	// ms
	int64       start_time; // ms
	int         signal;

	uint8	*pData_[4];
	int		lineSize_[4];
};

enum AVPixelFormat  RSFFMpegDecoder::hw_pix_fmt = AV_PIX_FMT_NONE;

RSFFMpegDecoder::RSFFMpegDecoder()
{
	pInputFormat_ = NULL;
	pAVFormatCtx_ = NULL;
	pAVCodecCtx_ = NULL;
	pAVFrame_ = NULL;
	pPacket_ = NULL;
	pSwsCtx_ = NULL;

	Init();
}

RSFFMpegDecoder::~RSFFMpegDecoder()
{
	cleanup();
}

void RSFFMpegDecoder::Init()
{
	av_register_all();
	avcodec_register_all();
}

void RSFFMpegDecoder::cleanup()
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

void RSFFMpegDecoder::SetMedia(HMedia hMedia)
{
	hMedia_ = hMedia;
}

int RSFFMpegDecoder::hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
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
enum AVPixelFormat RSFFMpegDecoder::get_hw_format(AVCodecContext *ctx,
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

int RSFFMpegDecoder::GetCodec()
{
	int iRet = 0;
	enum AVHWDeviceType type = AV_HWDEVICE_TYPE_VAAPI;
	/* AVHWDeviceType
		Intel: AV_HWDEVICE_TYPE_VAAPI
		Nvidia: AV_HWDEVICE_TYPE_CUDA
		Mac: AV_HWDEVICE_TYPE_VIDEOTOOLBOX
		Software: AV_HWDEVICE_TYPE_NONE
	*/

	getVideoSource();

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
				printf("hw supported pixel format %d\n", config->pix_fmt);
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
	video_time_base_num = video_stream->time_base.num;
	video_time_base_den = video_stream->time_base.den;

	if (video_stream->avg_frame_rate.den)
		fps = video_stream->avg_frame_rate.num / video_stream->avg_frame_rate.den;
	if (video_time_base_den && video_time_base_num) {
		duration = video_stream->duration / (double)video_time_base_den * video_time_base_num * 1000;
		start_time = video_stream->start_time / (double)video_time_base_den * video_time_base_num * 1000;
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
		pAVCodecCtx_->get_format = &RSFFMpegDecoder::get_hw_format;
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

int RSFFMpegDecoder::Decode()
{
	av_init_packet(pPacket_);
	int iRet = av_read_frame(pAVFormatCtx_, pPacket_);
	if (iRet != 0) {
		return 2001;
	}

	if (!pPacket_ || pPacket_->stream_index != video_stream_index) {
		return 2002;
	}

	if (avcodec_send_packet(pAVCodecCtx_, pPacket_) == 0) {
		while (avcodec_receive_frame(pAVCodecCtx_, pAVFrame_) == 0) {
			sws_scale(pSwsCtx_, pAVFrame_->data, pAVFrame_->linesize, 0, pAVFrame_->height, pData_, lineSize_);

			hFrame_.ts = pAVFrame_->pts / (double)video_time_base_den * video_time_base_num * 1000;
			hFrame_.userdata = pData_[0];

			//spHFrameBuf_->push(&hFrame_);	// restore into frame queue
		}
	}

	av_packet_unref(pPacket_);

	return 0;
}

int RSFFMpegDecoder::getVideoSource()
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
void RSFFMpegDecoder::makeFrame() {
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

int RSFFMpegDecoder::GetFps() {
	return fps;
}

#endif // __RS_FFMPEG_DECODER_H__
