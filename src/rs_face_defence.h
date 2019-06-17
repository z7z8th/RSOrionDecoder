#ifndef __RS_FACE_DEFENCE_H__
#define __RS_FACE_DEFENCE_H__

#include <string>

#include "hw/hthread.h"
#include "hw/hframe.h"
#include "rs_ffmpeg_decoder_task.h"
#include "rs_face_track_task.h"
#include "rs_image_upload_task.h"
#include "c1x_std.h"

class RSFaceDefence : public HThread {
public:
	RSFaceDefence();
	~RSFaceDefence() {}

	virtual int start();
	virtual int stop();
	virtual int pause() { return HThread::pause(); }
	virtual int resume() { return HThread::resume(); }

	void SetServerAddress(std::string address, std::string appKey, std::string appSecret);
	void SetFaceFilterPolicy(readsense::FilterPolicy &filter_policy);
	void SetMedia(HMedia &hMedia);

private:
	virtual void doTask();

private:
	std::shared_ptr<RSFFMpegDecoderTask>		sp_ffmpeg_decoder_task_;
	std::shared_ptr<RSFaceTrackTask>			sp_face_track_task_;
	std::shared_ptr<RSImageUploadTask>			sp_image_upload_task_;
};

RSFaceDefence::RSFaceDefence()
{
	sp_ffmpeg_decoder_task_ = std::make_shared<RSFFMpegDecoderTask>();
	sp_face_track_task_ = std::make_shared<RSFaceTrackTask>();
	sp_image_upload_task_ = std::make_shared<RSImageUploadTask>();

	sp_ffmpeg_decoder_task_->SetSpNextTask(sp_face_track_task_);
	sp_face_track_task_->SetSpNextTask(sp_image_upload_task_);
}

int RSFaceDefence::start()
{
	sp_ffmpeg_decoder_task_->Start();
	sp_face_track_task_->Start();
	sp_image_upload_task_->Start();

	return HThread::start();
}

int RSFaceDefence::stop()
{
	sp_ffmpeg_decoder_task_->Stop();
	sp_face_track_task_->Stop();
	sp_image_upload_task_->Stop();

	return HThread::stop();
}

void RSFaceDefence::doTask()
{
}

void RSFaceDefence::SetServerAddress(
	std::string address,
	std::string appKey,
	std::string appSecret)
{
	sp_image_upload_task_->Login(address, appKey, appSecret);
}

void RSFaceDefence::SetFaceFilterPolicy(
	readsense::FilterPolicy &filter_policy)
{
	sp_face_track_task_->SetFaceFilterPolicy(filter_policy);
}

void RSFaceDefence::SetMedia(HMedia &hMedia)
{
	sp_ffmpeg_decoder_task_->SetMedia(hMedia);
}

#endif // __RS_FACE_DEFENCE_H__
