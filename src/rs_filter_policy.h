#ifndef __RS_FILTER_POLICY_H__
#define __RS_FILTER_POLICY_H__

namespace readsense {

struct FrameTrack {
	int lastFrame;
	int trackCount;
};

struct RegionRatio {
	float xRatio = 0.0;
	float yRatio = 0.0;
	float widthRatio = 1.0;
	float heightRatio = 1.0;
};

struct FilterPolicy {
	RegionRatio _regionRatio;
	int	_faceWidthThreshold = 64;
	int	_faceHeightThreshold = 64;
	int	_faceQualityThreshold = 8;
	int	_faceAngleThreshold = 90;
	int	_onlySnapMaxFace = 0;
	int	_snapIntervalFrame = 2;
	int	_maxPituresPerTrack = 3;
};

}

#endif // __RS_FILTER_POLICY_H__