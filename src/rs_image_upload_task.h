#ifndef __RS_IMAGE_UPLOAD_TASK_H__
#define __RS_IMAGE_UPLOAD_TASK_H__

#include "hw/hlog.h"
#include "hw/hframe.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_image_upload.h"

class RSImageUploadTask : public RSImageUpload,
	public readsense::CRunnableAloneTaskList<readsense::RSFrame<uint8>> {
public:
	RSImageUploadTask();
	~RSImageUploadTask();

public:
	virtual void Run();
};

RSImageUploadTask::RSImageUploadTask() {
}

RSImageUploadTask::~RSImageUploadTask() {
}

void RSImageUploadTask::Run() {
	while (!stop_flag_) {
		readsense::RSFrame<uint8> rsFrame;
		rsFrame = _Pop();

		UploadImage(rsFrame.data(), rsFrame.width_, rsFrame.height_, "54:13:79:7e:dd:be");
	}

}

#endif // __RS_IMAGE_UPLOAD_TASK_H__
