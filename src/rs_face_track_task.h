#ifndef __RS_FACE_TRACK_TASK_H__
#define __RS_FACE_TRACK_TASK_H__

#include <map>
#include <atomic>

#include "hw/hlog.h"
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_face_track.h"
#include "rs_filter_policy.h"

class RSFaceTrackTask : public RSFaceTrack, 
	public readsense::CRunnableAloneTaskList<readsense::RSFrame<uint8>> {
public:
	RSFaceTrackTask();
	~RSFaceTrackTask();

public:
	virtual void Run();
};

RSFaceTrackTask::RSFaceTrackTask() {
}

RSFaceTrackTask::~RSFaceTrackTask() {
}

void RSFaceTrackTask::Run() {
	int				max = -1;
	int				maxi = -1;
	int				iRet = 0;
	int				facesNum = 0;
	HFrame			hFrameSrc;
	HFrame			hFrameDst;
	readsense::RSFrame<uint8> rsFrameSrc;
	readsense::RSFrame<uint8> rsFrameDst;
	rs_frame		frame;
	rs_face_info	*pFace = NULL;
	rs_face_info	*faces = NULL;

	while (!stop_flag_) {
		rsFrameSrc = _Pop();
		// hlogi("########FaceTrack Pop Success########");
		std::string src = rsFrameSrc.src_;

		iRet = GetRegionFrame(rsFrameSrc, rsFrameDst);

		frame.pix_fmt = PIX_FMT_BGR888;
		frame.height = rsFrameDst.height_;
		frame.width = rsFrameDst.width_;
		frame.pitch = rsFrameDst.width_ * 3;
		frame.data = rsFrameDst.data();

		iRet = FaceTrack(rsHandleFaceTrack_, &frame, &faces, &facesNum);
		//hlogi("$$$$$$$$FaceTrack ret: %d", iRet);
		if (iRet != 0 || facesNum <= 0) {
			continue;
		}

		//hlogi("########FaceTrack Success########");

		// filter policy
		int onlySnapMaxFace = filter_policy_._onlySnapMaxFace;

		for (int i = 0; i < facesNum; ++i) {
			pFace = &faces[i];

			if (onlySnapMaxFace) {

				if (max < pFace->rect.width) {
					max = pFace->rect.width;
					maxi = i;
				}
				continue;
			}

			hlogi("video[%s] FaceTrack trackid: %d, width=%d, height=%d, yaw=%f, pitch=%f, roll=%f",
				src.c_str(), pFace->trackId, pFace->rect.width, pFace->rect.height,
				pFace->yaw, pFace->pitch, pFace->roll);

			iRet = Filter(rsFrameDst, pFace);

			if (sp_next_task_ != nullptr) {
				if (!iRet) {
					sp_next_task_->Push(rsFramePush_);
				}
			}
		}

		if (maxi != -1) {
			pFace = &faces[maxi];
			hlogi("video[%s] onlySnapMaxFace###trackid: %d, width=%d, height=%d, yaw=%f, pitch=%f, roll=%f",
				src.c_str(), pFace->trackId, pFace->rect.width, pFace->rect.height,
				pFace->yaw, pFace->pitch, pFace->roll);

			Filter(rsFrameDst, pFace);

			if (sp_next_task_ != nullptr) {
				if (!iRet) {
					sp_next_task_->Push(rsFramePush_);
				}
			}
		}
	}
}

#endif // __RS_FACE_TRACK_TASK_H__
