#ifndef __RS_FFMPEG_DECODER_TASK_H__
#define __RS_FFMPEG_DECODER_TASK_H__

#include "hw/hlog.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_ffmpeg_decoder.h"

class RSFFMpegDecoderTask : public RSFFMpegDecoder,
	public readsense::CRunnableAloneTaskList<readsense::RSFrame<uint8>> {
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
	GetCodec();

	while (!stop_flag_) {
		av_init_packet(pPacket_);
		int iRet = av_read_frame(pAVFormatCtx_, pPacket_);
		//hlogi("av_read_frame, iRet: %d", iRet);
		if (iRet != 0) {
			continue;
		}

		if (!pPacket_ || pPacket_->stream_index != video_stream_index) {
			continue;
		}

		if (avcodec_send_packet(pAVCodecCtx_, pPacket_) == 0) {
			// 一个avPacket可能包含多帧数据，所以需要使用while循环一直读取
			while (avcodec_receive_frame(pAVCodecCtx_, pAVFrame_) == 0) {
				sws_scale(pSwsCtx_, pAVFrame_->data, pAVFrame_->linesize, 0, pAVFrame_->height, pData_, lineSize_);

				rsFrame_.ts_ = pAVFrame_->pts / (double)video_time_base_den * video_time_base_num * 1000;

				if (sp_next_task_ != nullptr) {
					sp_next_task_->Push(rsFrame_);
				}
			}
		}

		av_packet_unref(pPacket_);
	}
}

#endif // __RS_FFMPEG_DECODER_TASK_H__
