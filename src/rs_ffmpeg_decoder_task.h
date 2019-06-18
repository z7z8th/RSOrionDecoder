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
protected:
    virtual bool Output(std::shared_ptr<RSAVFramePacket> frame);
	virtual std::shared_ptr<RSAVFramePacket> Input();
	virtual bool ShouldStop() {
		return stop_flag_;
	}
public:
	virtual void Run();
};

RSFFMpegDecoderTask::RSFFMpegDecoderTask() {
}

RSFFMpegDecoderTask::~RSFFMpegDecoderTask() {
}

bool RSFFMpegDecoderTask::Output(std::shared_ptr<RSAVFramePacket> frame) {
	if (sp_next_task_ != nullptr) {
		sp_next_task_->Push(frame);
	}
    return sp_next_task_ != nullptr;
}

std::shared_ptr<RSAVFramePacket> RSFFMpegDecoderTask::Input() {
	// av_read_frame used instead of _Pop
	//return _Pop();
	return nullptr;
}

void RSFFMpegDecoderTask::Run() {
	if (!GetCodec()) {
		Decode();
	} else {
		hloge("GetCodec failed.\n");
		return;
	}
}

/* 
void scale_frame() {
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
}
*/

#endif // __RS_FFMPEG_DECODER_TASK_H__
