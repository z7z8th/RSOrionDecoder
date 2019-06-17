//#ifndef __RS_FACE_TRACK_V2_H__
//#define __RS_FACE_TRACK_V2_H__
//
////#ifdef _WIN32
////#pragma comment(lib, "libRSOrionDevice.lib")
////#endif
//
//#include <fstream>
//#include <sstream>
//#include <map>
//#include <atomic>
//
//#include "hw/hlog.h"
//#include "hw/hframe.h"
////#include "libreadsense/rsOrionDevice.h"
//#include "rs_filter_policy.h"
//#include "rs_image_process.h"
////#include "libreadface2/RSCommon.h"
////#include "libreadface2/RSFaceSDK2.h"
//#include "libreadface2/rsFaceSDKv1.h"
//#include "libreadsense/rs_face.h"
//#include "libreadsense/rs_base.h"
//#include "libreadsense/rs_image.h"
//
//
//#define CFG_INIT_LICENSE_PATH ""
//#define CFG_INIT_AUTH_CONTENT_PATH ""
//
//typedef void* HFaceTrack;
//
//class RSFaceTrack {
//public:
//	RSFaceTrack();
//	~RSFaceTrack();
//
//public:
//	int	SetFaceFilterPolicy(readsense::FilterPolicy &filter_policy);
//
//protected:
//	int init();
//	int uninit();
//
//	int		GetRegionFrame(readsense::RSFrame<uint8> &rsFrameSrc,
//		readsense::RSFrame<uint8> &rsFrameDst);
//	int		Filter(readsense::RSFrame<uint8> &rsFrame, rs_face_info* pFace);
//	bool	GetInitCfgInfo(std::string filePath, std::string& initInfo);
//	int		FaceTrack(HFaceTrack hFaceTrack, rs_frame* frame,
//		rs_face_info** faces, int* faces_num);
//
//protected:
//	RShandle		rsHandle_;
//	RShandle		rsHandleFaceTrack_;
//	HFrame			hFramePush_;
//	readsense::RSFrame<uint8> rsFramePush_;
//	//RSOrionHandle	rsoh_;
//	HFaceTrack      hFaceTrack_{ nullptr };
//
//	std::atomic<int>						current_frames_{ 0 };
//	readsense::FilterPolicy					filter_policy_;
//	std::map<int, readsense::FrameTrack>	mapFrameTrack_;
//};
//
//RSFaceTrack::RSFaceTrack()
//{
//	init();
//}
//
//RSFaceTrack::~RSFaceTrack()
//{
//	uninit();
//}
//
//int RSFaceTrack::init()
//{
//	//RSOrionInitParam param;
//	//memset(&param, 0, sizeof(param));
//	//const std::string appKey = "0d90c22236ffd9b7b9ee9b56bda9e024";
//	//const std::string appSecret = "c6fbaaae4f64ab4523f2211460ebcd285be6f1d2";
//	//strncpy(param.appId, appKey.c_str(), sizeof(param.appId));
//	//strncpy(param.appSecret, appSecret.c_str(), sizeof(param.appSecret));
//
//	//int ret = rsOrionInit(&rsoh_, &param);
//	//if (ret != 0) {
//	//	hloge("rsOrionInit failed: %d\n", ret);
//	//	return ret;
//	//}
//
//	//int iRet = rsOrionFaceTrackInit(rsoh_.hLicense, &hFaceTrack_);
//
//	std::string strLicenseContent("");
//	bool getLicFlag = GetInitCfgInfo(CFG_INIT_LICENSE_PATH, strLicenseContent);
//	if (!getLicFlag) {
//		return false;
//	}
//
//	rsInitLicenseManagerV2(&rsHandle_, strLicenseContent.c_str());
//
//	std::string strAuthContent("");
//	getLicFlag = GetInitCfgInfo(CFG_INIT_AUTH_CONTENT_PATH, strAuthContent);
//	if (!getLicFlag)
//	{
//		return false;
//	}
//	int activateFlag = rsActivateLicenseManagerV2(rsHandle_, strAuthContent.c_str());
//	printf("rsActivateLicenseManagerV2 activateFlag = %d \n", activateFlag);
//	if (activateFlag != 0) {
//		return -1;
//	}
//
//
//	rsInitFaceTrack(&rsHandleFaceTrack_, rsHandle_);
//	if (rsHandleFaceTrack_ == NULL)
//	{
//		return false;
//	}
//
//	return 0;
//}
//
//int RSFaceTrack::uninit()
//{
//	//int iRet = rsOrionFaceTrackUninit(&hFaceTrack_);
//	//iRet = rsOrionCleanup(&rsoh_);
//	return 0;
//}
//
//int RSFaceTrack::GetRegionFrame(readsense::RSFrame<uint8> &rsFrameSrc,
//	readsense::RSFrame<uint8> &rsFrameDst)
//{
//	int srcWidth = rsFrameSrc.width_;
//	int srcHeight = rsFrameSrc.height_;
//	int srcBpp = rsFrameSrc.bpp_;
//
//	rsFrameDst.type_ = 0x80E0;
//	rsFrameDst.width_ = srcWidth;
//	rsFrameDst.height_ = srcHeight;
//	rsFrameDst.bpp_ = 24;
//	int size = srcWidth * srcHeight * 24 / 8;
//	rsFrameDst.init(size);
//	memcpy(rsFrameDst.data(), rsFrameSrc.data(), size);
//
//	readsense::RegionRatio regionRatio = filter_policy_._regionRatio;
//
//	int x = static_cast<int>(regionRatio.xRatio * srcWidth);
//	int y = static_cast<int>(regionRatio.yRatio * srcHeight);
//	int width = static_cast<int>(regionRatio.widthRatio * srcWidth);
//	int height = static_cast<int>(regionRatio.heightRatio * srcHeight);
//	int pitch = width * 3;
//
//	rs_rect captureRegion = { x, y, width, height };
//	rs_rect frameRect = { 0, 0, srcWidth, srcHeight };
//
//	// capture rigion
//	if (x != 0 || y != 0 || width != srcWidth || height != srcHeight) {
//		rsFrameDst.type_ = 0x80E0;
//		rsFrameDst.width_ = width;
//		rsFrameDst.height_ = height;
//		rsFrameDst.bpp_ = 24;
//		int size = width * height * 24 / 8;
//		rsFrameDst.resize(size);
//
//		readsense::CutoutFaceImage(rsFrameSrc.data(), \
//			rsFrameDst.data(), captureRegion, frameRect);
//	}
//
//	return 0;
//}
//
//int	RSFaceTrack::SetFaceFilterPolicy(readsense::FilterPolicy &filter_policy)
//{
//	filter_policy_ = filter_policy;
//	return 0;
//}
//
//int RSFaceTrack::Filter(readsense::RSFrame<uint8> &rsFrame,
//	rs_face_info* pFace)
//{
//	readsense::FrameTrack frameTrack;
//
//	int faceWidthThreshold = filter_policy_._faceWidthThreshold;
//	int faceHeightThreshold = filter_policy_._faceHeightThreshold;
//	int faceAngleThreshold = filter_policy_._faceAngleThreshold;
//	int snapIntervalFrame = filter_policy_._snapIntervalFrame;
//	int maxPituresPerTrack = filter_policy_._maxPituresPerTrack;
//
//	++current_frames_;
//	int	trackId = pFace->trackId;
//	auto iter = mapFrameTrack_.find(trackId);
//
//	if (pFace->rect.width >= faceWidthThreshold && \
//		pFace->rect.height >= faceHeightThreshold && \
//		ABS(pFace->yaw) < faceAngleThreshold &&	\
//		ABS(pFace->pitch) < faceAngleThreshold && \
//		ABS(pFace->roll) < faceAngleThreshold) {
//
//		if (iter == mapFrameTrack_.end()) {
//			frameTrack = { current_frames_, 0 };
//			mapFrameTrack_[trackId] = frameTrack;
//			iter = mapFrameTrack_.find(trackId);
//		}
//
//		frameTrack = iter->second;
//		hlogi("trackid=%d, current_frames=%d, last_frame=%d, \
//			track_count=%d, snap_interval_frame_=%d, max_pitures_per_track_=%d", \
//			trackId, current_frames_, frameTrack.lastFrame, frameTrack.trackCount, \
//			snapIntervalFrame, maxPituresPerTrack);
//
//		if ((current_frames_ != frameTrack.lastFrame) &&
//			(current_frames_ - frameTrack.lastFrame <= snapIntervalFrame)) {
//			hlogi("trackid=%d, current_frames=%d, last_frame=%d, snap_interval_frame_=%d",
//				trackId, current_frames_, frameTrack.lastFrame, snapIntervalFrame);
//			return 1000;		// in snap interval frames
//		}
//
//		/* if max_pitures_per_track_ set */
//		if (maxPituresPerTrack) {
//			if (frameTrack.trackCount >= maxPituresPerTrack) {
//				hlogi("trackid=%d, current_frames=%d, track_count=%d, max_pitures_per_track_=%d",
//					trackId, current_frames_, frameTrack.trackCount, maxPituresPerTrack);
//				return 1001;
//			}
//		}
//
//		frameTrack.lastFrame = current_frames_;
//		frameTrack.trackCount++;
//		mapFrameTrack_[trackId] = frameTrack;
//
//		// shape face rect
//		rs_rect frameRect = { 0, 0, rsFrame.width_, rsFrame.height_ };
//		rs_rect faceRect = readsense::CutoutFaceRect(frameRect, pFace->rect);
//
//		rsFramePush_.width_ = faceRect.width;
//		rsFramePush_.height_ = faceRect.height;
//		rsFramePush_.type_ = 0x80E0;		//GL_BGR;
//		rsFramePush_.bpp_ = 24;
//		int size = rsFramePush_.width_ * rsFramePush_.height_ * rsFramePush_.bpp_ / 8;
//		rsFramePush_.init(size);
//		readsense::CutoutFaceImage(rsFrame.data(), rsFramePush_.data(), faceRect, frameRect);
//
//		return 0;
//	}
//
//	return 1003;
//}
//
//bool RSFaceTrack::GetInitCfgInfo(std::string filePath, std::string& initInfo)
//{
//	std::ifstream filestream(filePath.c_str());
//	if (!filestream.is_open())
//	{
//		return false;
//	}
//	std::stringstream streambuf;
//	streambuf << filestream.rdbuf();
//	initInfo = streambuf.str();
//	return true;
//}
//
//int RSFaceTrack::FaceTrack(HFaceTrack hFaceTrack, rs_frame* frame,
//	rs_face_info** faces, int* faces_num) {
//	return rsRunFaceTrack(hFaceTrack, frame->data, frame->pix_fmt, frame->width, frame->height,
//		0, RS_IMG_CLOCKWISE_ROTATE_0, faces, faces_num);
//}
//#endif // __RS_FACE_TRACK_V2_H__
