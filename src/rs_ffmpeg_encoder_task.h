#ifndef __RS_FFMPEG_ENCODER_TASK_H__
#define __RS_FFMPEG_ENCODER_TASK_H__

#include <iostream>
#include "hw/hlog.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_ffmpeg_encoder.h"

class RSFFMpegEncoderTask : public RSFFMpegEncoder,
    public readsense::CRunnableAloneTaskList<std::shared_ptr<RSAVFramePacket>>
{
public:
	RSFFMpegEncoderTask();
	virtual ~RSFFMpegEncoderTask();
protected:
    virtual bool Output(std::shared_ptr<RSAVFramePacket> frame);
	virtual std::shared_ptr<RSAVFramePacket> Input();
	virtual bool ShouldStop() {
		return stop_flag_;
	}
public:
	virtual void Run();
};

RSFFMpegEncoderTask::RSFFMpegEncoderTask() {
}

RSFFMpegEncoderTask::~RSFFMpegEncoderTask() {
}

bool RSFFMpegEncoderTask::Output(std::shared_ptr<RSAVFramePacket> frame) {
	if (sp_next_task_ != nullptr) {
		sp_next_task_->Push(frame);
	}
    return sp_next_task_ != nullptr;
}

std::shared_ptr<RSAVFramePacket> RSFFMpegEncoderTask::Input() {
	return _Pop();
}

void RSFFMpegEncoderTask::Run() {
    {
        std::shared_ptr<RSAVFramePacket> avdata = _Peek();
        width_ = avdata->frame_->width;
        height_ = avdata->frame_->height;
        src_ = avdata->src_;
    }
    Init();
    if (!GetCodec()) {
        Encode();
    } else {
        std::cout << "Get encode codec failed.\n" << std::endl;
    }
}

#endif //__RS_FFMPEG_ENCODER_H__