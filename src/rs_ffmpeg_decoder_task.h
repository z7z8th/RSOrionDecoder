#ifndef __RS_FFMPEG_DECODER_TASK_H__
#define __RS_FFMPEG_DECODER_TASK_H__

#include "hw/hlog.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_ffmpeg_decoder.h"
#include "rs_avframe_cxx.h"

class RSFFMpegDecoderTask : public RSFFMpegDecoder,
	public readsense::CRunnableAloneTaskList<std::shared_ptr<RSAVFramePacket>>
{
public:
	RSFFMpegDecoderTask();
	~RSFFMpegDecoderTask();

public:
	virtual void Run();
};

RSFFMpegDecoderTask::RSFFMpegDecoderTask() {
}

RSFFMpegDecoderTask::~RSFFMpegDecoderTask() {
}

void RSFFMpegDecoderTask::Run() {
	int ret = -1;
	AVFrame *sw_frame = NULL;
    AVFrame *tmp_frame = NULL;

	if (GetCodec()) {
		hloge("GetCodec failed.\n");
		return;
	}
	if (!(pAVFrame_ = av_frame_alloc())){
		fprintf(stderr, "Can not alloc frame\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}

	while (!stop_flag_) {

		//av_init_packet(pPacket_);
		int iRet = av_read_frame(pAVFormatCtx_, pPacket_);
		hlogi("av_read_frame, iRet: 0x%x", iRet);
		if (iRet != 0) {
			usleep(100*1000);
			continue;
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
/* 
				if (!pSwsCtx_)
				{
					int sw = pAVCodecCtx_->width;
					int sh = pAVCodecCtx_->height;
					int dw = sw & ~0x03L; // align = 4
					int dh = sh;
					pSwsCtx_ = sws_getContext(sw, sh, (AVPixelFormat) tmp_frame->format, dw, dh,
											  dstPixelFormat_, SWS_FAST_BILINEAR, NULL, NULL, NULL);
				}
				int height = sws_scale(pSwsCtx_, tmp_frame->data, tmp_frame->linesize, 0, tmp_frame->height, pData_, lineSize_);
				printf("sws_scale ret height %d\n", height);

				rsFrame_.ts_ = tmp_frame->pts / (double)video_time_base_den * video_time_base_num * 1000;
				ConvertFormat(tmp_frame, &rsFrame_);
				rsFrame_.ts_ = tmp_frame->pts / (double)video_time_base_den * video_time_base_num * 1000;
 */

				//Output(rsFrame);

				if (sp_next_task_ != nullptr) {
					//printf("push frame fmt %d size %dx%d\n", tmp_frame->format, tmp_frame->width, tmp_frame->height);
					std::shared_ptr<RSAVFramePacket> avframe(new RSAVFramePacket());
					avframe->AllocFrame().FrameMoveRefFrom(tmp_frame);
					avframe->src_ = hMedia_.src;
					sp_next_task_->Push(avframe);
				} else {
					av_frame_unref(tmp_frame);
				}
			}
		}

		av_packet_unref(pPacket_);
	}
fail:
	return ;
}

#endif // __RS_FFMPEG_DECODER_TASK_H__
