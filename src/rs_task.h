#ifndef __RS_TASK_H__
#define __RS_TASK_H__

#ifdef _WIN32
#pragma comment(lib, "libRSOrionDevice.lib")
#endif

#include "hw/hlog.h"
#include "hw/hframe.h"
#include "libreadsense/rsOrionDevice.h"

class RSTask {
public:
	RSTask();
	~RSTask();

public:
	virtual int Execute();

	virtual void SetFrameBufSrc(std::shared_ptr<HFrameBuf> spHFrameBufSrc);
	virtual void SetFrameBufDst(std::shared_ptr<HFrameBuf> spHFrameBufDst);

protected:
	virtual int init();
	virtual int uninit();

protected:
	std::shared_ptr<HFrameBuf>	spHFrameBufSrc_;
	std::shared_ptr<HFrameBuf>	spHFrameBufDst_;

	RSOrionHandle	rsoh_;
};

RSTask::RSTask()
{
	init();
}

RSTask::~RSTask()
{
	uninit();
}

int RSTask::init()
{
	RSOrionInitParam param;
	memset(&param, 0, sizeof(param));
	const std::string appKey = "0d90c22236ffd9b7b9ee9b56bda9e024";
	const std::string appSecret = "c6fbaaae4f64ab4523f2211460ebcd285be6f1d2";
	strncpy(param.appId, appKey.c_str(), sizeof(param.appId));
	strncpy(param.appSecret, appSecret.c_str(), sizeof(param.appSecret));

	int ret = rsOrionInit(&rsoh_, &param);
	if (ret != 0) {
		hloge("rsOrionInit failed: %d\n", ret);
	}

	return ret;
}

int RSTask::uninit()
{
	rsOrionCleanup(&rsoh_);
	return 0;
}

void RSTask::SetFrameBufSrc(std::shared_ptr<HFrameBuf> spHFrameBufSrc)
{
	spHFrameBufSrc_ = spHFrameBufSrc;
}

void RSTask::SetFrameBufDst(std::shared_ptr<HFrameBuf> spHFrameBufDst)
{
	spHFrameBufDst_ = spHFrameBufDst;
}

int RSTask::Execute()
{
	// todo: specific algorithm decides

	return 0;
}
#endif	// __RS_TASK_H__
