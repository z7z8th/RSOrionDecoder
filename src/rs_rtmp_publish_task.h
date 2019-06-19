#ifndef __RS_RTMP_PUBLISH_TASK_H__
#define __RS_RTMP_PUBLISH_TASK_H__

#include <iostream>
#include "hw/hlog.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_rtmp_publish.h"

class RSRtmpPublishTask : public RSRtmpPublish,
    public readsense::CRunnableAloneTaskList<std::shared_ptr<RSAVFramePacket>>
{
public:
	RSRtmpPublishTask();
	virtual ~RSRtmpPublishTask();
protected:
	bool Output(std::shared_ptr<RSAVFramePacket> frame) {
		if (sp_next_task_ != nullptr) {
			sp_next_task_->Push(frame);
		}
		return sp_next_task_ != nullptr;
	}
	std::shared_ptr<RSAVFramePacket> Input() {
		return _Pop();
	}
	virtual bool ShouldStop() {
		return stop_flag_;
	}
public:
	virtual void Run();
};

RSRtmpPublishTask::RSRtmpPublishTask() {
}

RSRtmpPublishTask::~RSRtmpPublishTask() {
}

void RSRtmpPublishTask::Run() {
    if (!ConnectRtmpServer()) {
        PublishStream();
    } else {
        std::cout << "ConnectRtmpServer Failed." << std::endl;
    }

	FinishTaskChain();
	OutputDateTime();
    std::cout << "RSRtmpPublish::Run exited." << std::endl;
}

#endif //__RS_RTMP_PUBLISH_TASK_H__