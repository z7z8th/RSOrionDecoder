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
    bool Output(std::shared_ptr<RSAVFramePacket> frame) {
        if (sp_next_task_ != nullptr) {
            sp_next_task_->Push(frame);
        }
        return sp_next_task_ != nullptr;
    }	
    virtual std::shared_ptr<RSAVFramePacket> Input() {
        return _Pop();
    }
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

	FinishTaskChain();
    OutputDateTime();
    std::cout << "RSFFMpegEncoderTask::Run exited." << std::endl;
}

#endif //__RS_FFMPEG_ENCODER_H__