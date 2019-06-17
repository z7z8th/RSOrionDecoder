#ifndef __RS_FACE_TRACK_H__
#define __RS_FACE_TRACK_H__

#ifdef _WIN32
#pragma comment(lib, "libReadFace2.lib")
#endif

#include <fstream>
#include <sstream>
#include <map>
#include <atomic>

#include "hw/hlog.h"
#include "hw/hframe.h"
#include "rs_filter_policy.h"
#include "rs_image_process.h"
#include "libreadface2/rsFaceSDKv1.h"

#define CFG_INIT_LICENSE_PATH "d456daf2_x86_linux_offline_license_content.lic"
#define CFG_INIT_AUTH_CONTENT_PATH "d456daf2_x86_linux_offline_license_content.lic_device_key_content.txt_auth.txt"

typedef void* HFaceTrack;

class RSFaceTrack {
public:
	RSFaceTrack();
	~RSFaceTrack();

public:
	int	SetFaceFilterPolicy(readsense::FilterPolicy &filter_policy);

protected:
	int init();
	int uninit();

	int		GetRegionFrame(readsense::RSFrame<uint8> &rsFrameSrc,
		readsense::RSFrame<uint8> &rsFrameDst);
	int		Filter(readsense::RSFrame<uint8> &rsFrame, rs_face_info* pFace);
	bool	GetInitCfgInfo(std::string filePath, std::string& initInfo);
	int		FaceTrack(HFaceTrack hFaceTrack, rs_frame* frame,
		rs_face_info** faces, int* faces_num);

protected:
	RShandle		rsHandle_;
	RShandle		rsHandleFaceTrack_;
	readsense::RSFrame<uint8> rsFramePush_;
	HFaceTrack      hFaceTrack_{ nullptr };

	std::atomic<int>						current_frames_{ 0 };
	readsense::FilterPolicy					filter_policy_;
	std::map<int, readsense::FrameTrack>	mapFrameTrack_;
};

RSFaceTrack::RSFaceTrack()
{
	init();
}

RSFaceTrack::~RSFaceTrack()
{
	uninit();
}

int RSFaceTrack::init()
{
	std::string strLicenseContent("");
	bool getLicFlag = GetInitCfgInfo(CFG_INIT_LICENSE_PATH, strLicenseContent);
	hlogi("GetInitCfgInfo getLicFlag: %d", getLicFlag);
	if (!getLicFlag) {
		return false;
	}

	rsInitLicenseManagerV2(&rsHandle_, strLicenseContent.c_str());

	std::string strAuthContent("");
	getLicFlag = GetInitCfgInfo(CFG_INIT_AUTH_CONTENT_PATH, strAuthContent);
	if (!getLicFlag)
	{
		return false;
	}
	int activateFlag = rsActivateLicenseManagerV2(rsHandle_, strAuthContent.c_str());
	hlogi("rsActivateLicenseManagerV2 activateFlag = %d", activateFlag);
	if (activateFlag != 0) {
		return -1;
	}


	rsInitFaceTrack(&rsHandleFaceTrack_, rsHandle_);
	if (rsHandleFaceTrack_ == NULL)
	{
		return false;
	}

	return 0;
}

int RSFaceTrack::uninit()
{
	rsUnInitFaceTrack(&rsHandleFaceTrack_);
	rsUnInitLicenseManagerV2(&rsHandle_);
	return 0;
}

int RSFaceTrack::GetRegionFrame(readsense::RSFrame<uint8> &rsFrameSrc,
	readsense::RSFrame<uint8> &rsFrameDst)
{
	int srcWidth = rsFrameSrc.width_;
	int srcHeight = rsFrameSrc.height_;
	int srcBpp = rsFrameSrc.bpp_;

	rsFrameDst.type_ = 0x80E0;
	rsFrameDst.width_ = srcWidth;
	rsFrameDst.height_ = srcHeight;
	rsFrameDst.bpp_ = 24;
	int size = srcWidth * srcHeight * 24 / 8;
	rsFrameDst.init(size);
	memcpy(rsFrameDst.data(), rsFrameSrc.data(), size);

	readsense::RegionRatio regionRatio = filter_policy_._regionRatio;

	int x = static_cast<int>(regionRatio.xRatio * srcWidth);
	int y = static_cast<int>(regionRatio.yRatio * srcHeight);
	int width = static_cast<int>(regionRatio.widthRatio * srcWidth);
	int height = static_cast<int>(regionRatio.heightRatio * srcHeight);
	int pitch = width * 3;

	rs_rect captureRegion = { x, y, width, height };
	rs_rect frameRect = { 0, 0, srcWidth, srcHeight };

	// capture rigion
	if (x != 0 || y != 0 || width != srcWidth || height != srcHeight) {
		rsFrameDst.type_ = 0x80E0;
		rsFrameDst.width_ = width;
		rsFrameDst.height_ = height;
		rsFrameDst.bpp_ = 24;
		int size = width * height * 24 / 8;
		rsFrameDst.resize(size);

		readsense::CutoutFaceImage(rsFrameSrc.data(), \
			rsFrameDst.data(), captureRegion, frameRect);
	}

	return 0;
}

int	RSFaceTrack::SetFaceFilterPolicy(readsense::FilterPolicy &filter_policy)
{
	filter_policy_ = filter_policy;
	return 0;
}

int RSFaceTrack::Filter(readsense::RSFrame<uint8> &rsFrame,
	rs_face_info* pFace)
{
	readsense::FrameTrack frameTrack;

	int faceWidthThreshold = filter_policy_._faceWidthThreshold;
	int faceHeightThreshold = filter_policy_._faceHeightThreshold;
	int faceAngleThreshold = filter_policy_._faceAngleThreshold;
	int snapIntervalFrame = filter_policy_._snapIntervalFrame;
	int maxPituresPerTrack = filter_policy_._maxPituresPerTrack;

	++current_frames_;
	int	trackId = pFace->trackId;
	auto iter = mapFrameTrack_.find(trackId);

	if (pFace->rect.width >= faceWidthThreshold && \
		pFace->rect.height >= faceHeightThreshold && \
		ABS(pFace->yaw) < faceAngleThreshold &&	\
		ABS(pFace->pitch) < faceAngleThreshold && \
		ABS(pFace->roll) < faceAngleThreshold) {

		if (iter == mapFrameTrack_.end()) {
			frameTrack = { current_frames_, 0 };
			mapFrameTrack_[trackId] = frameTrack;
			iter = mapFrameTrack_.find(trackId);
		}

		frameTrack = iter->second;
		//hlogi("trackid=%d, current_frames=%d, last_frame=%d, \
			track_count=%d, snap_interval_frame_=%d, max_pitures_per_track_=%d", \
			trackId, current_frames_, frameTrack.lastFrame, frameTrack.trackCount, \
			snapIntervalFrame, maxPituresPerTrack);

		if ((current_frames_ != frameTrack.lastFrame) &&
			(current_frames_ - frameTrack.lastFrame <= snapIntervalFrame)) {
			//hlogi("trackid=%d, current_frames=%d, last_frame=%d, snap_interval_frame_=%d", \
				trackId, current_frames_, frameTrack.lastFrame, snapIntervalFrame);
			return 1000;		// in snap interval frames
		}

		/* if max_pitures_per_track_ set */
		if (maxPituresPerTrack) {
			if (frameTrack.trackCount >= maxPituresPerTrack) {
				//hlogi("trackid=%d, current_frames=%d, track_count=%d, max_pitures_per_track_=%d", \
					trackId, current_frames_, frameTrack.trackCount, maxPituresPerTrack);
				return 1001;
			}
		}

		frameTrack.lastFrame = current_frames_;
		frameTrack.trackCount++;
		mapFrameTrack_[trackId] = frameTrack;

		// shape face rect
		rs_rect frameRect = { 0, 0, rsFrame.width_, rsFrame.height_ };
		rs_rect faceRect = readsense::CutoutFaceRect(frameRect, pFace->rect);

		rsFramePush_.width_ = faceRect.width;
		rsFramePush_.height_ = faceRect.height;
		rsFramePush_.type_ = 0x80E0;		//GL_BGR;
		rsFramePush_.bpp_ = 24;
		int size = rsFramePush_.width_ * rsFramePush_.height_ * rsFramePush_.bpp_ / 8;
		rsFramePush_.init(size);
		readsense::CutoutFaceImage(rsFrame.data(), rsFramePush_.data(), faceRect, frameRect);

		return 0;
	}

	return 1003;
}

bool RSFaceTrack::GetInitCfgInfo(std::string filePath, std::string& initInfo)
{
	std::ifstream filestream(filePath.c_str());
	if (!filestream.is_open()) {
		return false;
	}
	std::stringstream streambuf;
	streambuf << filestream.rdbuf();
	initInfo = streambuf.str();
	return true;
}

int RSFaceTrack::FaceTrack(HFaceTrack hFaceTrack, rs_frame* frame,
	rs_face_info** faces, int* faces_num) {
	return rsRunFaceTrack(hFaceTrack, frame->data, frame->pix_fmt,
						  frame->width, frame->height,frame->pitch, 
						  RS_IMG_CLOCKWISE_ROTATE_0, faces, faces_num);
}
#endif // __RS_FACE_TRACK_V2_H__
