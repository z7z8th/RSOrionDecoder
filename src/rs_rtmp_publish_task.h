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
    virtual bool Output(std::shared_ptr<RSAVFramePacket> frame);
	virtual std::shared_ptr<RSAVFramePacket> Input();

public:
	virtual void Run();
};

RSRtmpPublishTask::RSRtmpPublishTask() {
}

RSRtmpPublishTask::~RSRtmpPublishTask() {
}

bool RSRtmpPublishTask::Output(std::shared_ptr<RSAVFramePacket> frame) {
	if (sp_next_task_ != nullptr) {
		sp_next_task_->Push(frame);
	}
    return sp_next_task_ != nullptr;
}

std::shared_ptr<RSAVFramePacket> RSRtmpPublishTask::Input() {
	return _Pop();
}

void RSRtmpPublishTask::Run() {
    if (!ConnectRtmpServer()) {
        PublishStream();
    } else {
        std::cout << "ConnectRtmpServer Failed." << std::endl;
    }
}

#endif //__RS_RTMP_PUBLISH_TASK_H__