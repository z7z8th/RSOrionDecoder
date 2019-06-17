#ifndef __RS_ORION_DEFENCE_H__
#define __RS_ORION_DEFENCE_H__

#include <list>
#include <string>
#include <memory>
#include <chrono>

#include "rs_face_defence.h"

typedef struct VideoHandler {
	std::string _videoSrc;
	std::shared_ptr<RSFaceDefence> _spRSFaceDefence;
};

class RSOrionDefence {
public:
	RSOrionDefence();
	~RSOrionDefence();

public:
	int		AddVideoSource(std::string videoSource, bool bNetwork = true);
	int		DelVideoSource(std::string videoSource);
	std::list<std::string> GetVideoSourceList();

	void	SetDeviceInfo();
	void	SetDecodeMode(int decodeMode);
	int		SetFaceFilterPolicy(readsense::FilterPolicy &fileter_policy);
	void	SetServerAddress(std::string address, std::string appKey, std::string appSecret);

	int  Start();
	void Stop();

private:
	std::atomic<int> currentVideoNum_{ 0 };
	std::list<std::shared_ptr<VideoHandler>> listVideoHandlerPtr_;
};

RSOrionDefence::RSOrionDefence() = default;
RSOrionDefence::~RSOrionDefence() = default;

int RSOrionDefence::AddVideoSource(std::string videoSource, bool bNetwork)
{
	HMedia hMedia;
	hMedia.src = videoSource;
	hMedia.type = bNetwork ? readsense::MEDIA_TYPE_NETWORK : readsense::MEDIA_TYPE_CAPTURE;

	std::shared_ptr<RSFaceDefence> spRSFaceDefence = std::make_shared<RSFaceDefence>();
	spRSFaceDefence->SetMedia(hMedia);

	std::shared_ptr<VideoHandler> spVideoHandler = std::make_shared<VideoHandler>();

	spVideoHandler->_videoSrc = videoSource;
	spVideoHandler->_spRSFaceDefence = spRSFaceDefence;
	listVideoHandlerPtr_.emplace_back(spVideoHandler);

	++currentVideoNum_;

	return currentVideoNum_;
}

int RSOrionDefence::DelVideoSource(std::string videoSource)
{
	for (auto &pVideoHandlerPtr : listVideoHandlerPtr_) {
		if (pVideoHandlerPtr->_videoSrc.compare(videoSource) == 0) {
			pVideoHandlerPtr->_spRSFaceDefence->stop();
			listVideoHandlerPtr_.remove(pVideoHandlerPtr);
			break;
		}
	}

	--currentVideoNum_;

	return currentVideoNum_;
}

std::list<std::string> RSOrionDefence::GetVideoSourceList()
{
	std::list<std::string> listVideoSrc;

	for (auto &pVideoHandlerPtr : listVideoHandlerPtr_) {
		listVideoSrc.emplace_back(pVideoHandlerPtr->_videoSrc);
	}
	return listVideoSrc;
}

void RSOrionDefence::SetDecodeMode(int decodeMode)
{

}

int	RSOrionDefence::SetFaceFilterPolicy(
	readsense::FilterPolicy &fileter_policy) {
	for (auto &pVideoHandlerPtr : listVideoHandlerPtr_) {
		pVideoHandlerPtr->_spRSFaceDefence->SetFaceFilterPolicy(
			fileter_policy);
	}
	return 0;
}

void RSOrionDefence::SetDeviceInfo()
{

}

void RSOrionDefence::SetServerAddress(std::string address,
	std::string appKey, std::string appSecret)
{
	for (auto &pVideoHandlerPtr : listVideoHandlerPtr_) {
		pVideoHandlerPtr->_spRSFaceDefence->SetServerAddress(
			address, appKey, appSecret);
	}

}

int RSOrionDefence::Start()
{
	for (auto &pVideoHandlerPtr : listVideoHandlerPtr_) {
		pVideoHandlerPtr->_spRSFaceDefence->start();
	}

	while (1) {
		if (true) {
			//std::this_thread::yield();
			std::this_thread::sleep_for(std::chrono::seconds(60));
		}
	}

	return 0;
}

void RSOrionDefence::Stop()
{
	for (auto &pVideoHandlerPtr : listVideoHandlerPtr_) {
		pVideoHandlerPtr->_spRSFaceDefence->stop();
	}
}

#endif // __RS_ORION_DEFENCE_H__
