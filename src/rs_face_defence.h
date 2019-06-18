#ifndef __RS_FACE_DEFENCE_H__
#define __RS_FACE_DEFENCE_H__

#include <string>

//#define FACE_TRACK

#include "hw/hthread.h"
#include "hw/hframe.h"
#include "rs_ffmpeg_decoder_task.h"
#include "rs_ffmpeg_encoder_task.h"
#ifdef FACE_TRACK
#include "rs_face_track_task.h"
#include "rs_image_upload_task.h"
#endif
#include "rs_dump_frames_task.h"
#include "rs_rtmp_publish_task.h"
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
	void SetRtmpServer(const std::string &rtmp_server, const std::string &path, int index);

private:
	virtual void doTask();

private:
	std::shared_ptr<RSFFMpegDecoderTask>		sp_ffmpeg_decoder_task_;
#ifdef FACE_TRACK
	std::shared_ptr<RSFaceTrackTask>			sp_face_track_task_;
	std::shared_ptr<RSImageUploadTask>			sp_image_upload_task_;
#endif
	std::shared_ptr<RSDumpFramesTask>			sp_dump_decoded_task_;
	std::shared_ptr<RSFFMpegEncoderTask>		sp_ffmpeg_encoder_task_;
	std::shared_ptr<RSDumpFramesTask>			sp_dump_encoded_task_;
	std::shared_ptr<RSRtmpPublishTask>			sp_rtmp_publish_task_;

	const bool DUMP_DECODED_FRAMES = true;
	const bool DUMP_ENCODED_PACKETS = true;
	const bool DISCARD_DUMP_DATA = true;
};

RSFaceDefence::RSFaceDefence()
{
	sp_ffmpeg_decoder_task_ = std::make_shared<RSFFMpegDecoderTask>();
#ifdef FACE_TRACK
	sp_face_track_task_ = std::make_shared<RSFaceTrackTask>();
	sp_image_upload_task_ = std::make_shared<RSImageUploadTask>();
#endif
	sp_ffmpeg_encoder_task_ = std::make_shared<RSFFMpegEncoderTask>();
	sp_rtmp_publish_task_ = std::make_shared<RSRtmpPublishTask>();

	if (DUMP_DECODED_FRAMES) {
		sp_dump_decoded_task_ = std::make_shared<RSDumpFramesTask>("decoded-", "");
		sp_dump_decoded_task_->DiscardData(DISCARD_DUMP_DATA);

		sp_ffmpeg_decoder_task_->SetSpNextTask(sp_dump_decoded_task_);
		sp_dump_decoded_task_->SetSpNextTask(sp_ffmpeg_encoder_task_);
	} else {
		sp_ffmpeg_decoder_task_->SetSpNextTask(sp_ffmpeg_encoder_task_);
	}
	//sp_ffmpeg_decoder_task_->SetSpNextTask(sp_ffmpeg_encoder_task_);
	if (DUMP_ENCODED_PACKETS) {
		sp_dump_encoded_task_ = std::make_shared<RSDumpFramesTask>("encoded-", "h264");
		sp_dump_encoded_task_->DiscardData(DISCARD_DUMP_DATA);
		
		sp_ffmpeg_encoder_task_->SetSpNextTask(sp_dump_encoded_task_);
		sp_dump_encoded_task_->SetSpNextTask(sp_rtmp_publish_task_);
	} else {
		sp_ffmpeg_encoder_task_->SetSpNextTask(sp_rtmp_publish_task_);
	}
	/*
	sp_ffmpeg_decoder_task_->SetSpNextTask(sp_face_track_task_);
	sp_face_track_task_->SetSpNextTask(sp_image_upload_task_);
	*/
}

int RSFaceDefence::start()
{
	sp_ffmpeg_decoder_task_->Start();
	if (DUMP_DECODED_FRAMES)
		sp_dump_decoded_task_->Start();
	sp_ffmpeg_encoder_task_->Start();
	if (DUMP_ENCODED_PACKETS)
		sp_dump_encoded_task_->Start();
	sp_rtmp_publish_task_->Start();
	/*
	sp_face_track_task_->Start();
	sp_image_upload_task_->Start();
	*/

	return HThread::start();
}

int RSFaceDefence::stop()
{
	sp_ffmpeg_decoder_task_->Stop();
	if (DUMP_DECODED_FRAMES)
		sp_dump_decoded_task_->Stop();
/* 	sp_face_track_task_->Stop();
	sp_image_upload_task_->Stop();
 */	
	sp_ffmpeg_encoder_task_->Stop();
	if (DUMP_ENCODED_PACKETS)
		sp_dump_encoded_task_->Stop();

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
#ifdef FACE_TRACK
	sp_image_upload_task_->Login(address, appKey, appSecret);
#endif
}

void RSFaceDefence::SetFaceFilterPolicy(
	readsense::FilterPolicy &filter_policy)
{
#ifdef FACE_TRACK
	sp_face_track_task_->SetFaceFilterPolicy(filter_policy);
#endif
}

void RSFaceDefence::SetMedia(HMedia &hMedia)
{
	sp_ffmpeg_decoder_task_->SetMedia(hMedia);
}

void RSFaceDefence::SetRtmpServer(const std::string &rtmp_server, const std::string &path, int index){
	sp_rtmp_publish_task_->SetRtmpServer(rtmp_server, path, index);
}

#endif // __RS_FACE_DEFENCE_H__
