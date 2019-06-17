#include <string>
#include <stdio.h>
#include <unistd.h>
#include "RSOrionDecoder.h"

#ifdef _WIN32
#pragma comment(lib, "RSOrionDecoder.lib")
#endif

int main()
{
	std::string videoSource171("rtsp://10.1.12.171:554/media/live/1/1");
	std::string videoSource234("rtsp://10.1.12.234:554/media/live/1/1");
	std::string videoSource67("rtsp://10.1.12.67:554/media/live/1/1");
	std::string videoSource17("rtsp://10.1.12.17:554/media/live/1/1");
	std::string videoSource16("rtsp://10.1.12.16:554/media/live/1/1");
	std::string videoSource18("rtsp://10.1.12.18:554/media/live/1/1");
	std::string videoSource247("rtsp://10.1.12.247:554/media/live/1/1");
	std::string videoSource248("rtsp://10.1.12.248:554/media/live/1/1");
	std::string videoSource235("rtsp://10.1.12.235:554/media/live/1/1");
	std::string videoSource151("rtsp://10.1.12.151:554/media/live/1/1");
	std::string videoSourceMp4("/home/chengwei/code/video/test.mp4");

	std::string address = "39.106.21.118";
	const std::string appKey = "29ae8ff0386f4810";
	const std::string appSecret = "39a81d0d7d0e4948";

	readsense::RegionRatio regionRatio;
	regionRatio.xRatio = 0.0;
	regionRatio.yRatio = 0.0;
	regionRatio.widthRatio = 1.0;
	regionRatio.heightRatio = 1.0;

	readsense::FilterPolicy filter_policy;
	filter_policy._regionRatio = regionRatio;
	filter_policy._maxPituresPerTrack = 5;


	//AddVideoSource(rsHandle, videoSourceMp4);
	/*
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	   AddVideoSource(rsHandle, videoSource171);
	 */
	//AddVideoSource(rsHandle, videoSource171);
	//AddVideoSource(rsHandle, videoSource171);
	//AddVideoSource(rsHandle, videoSource248);
	//AddVideoSource(rsHandle, videoSource248);
	//AddVideoSource(rsHandle, videoSource248);
	//AddVideoSource(rsHandle, videoSource248);
	//AddVideoSource(rsHandle, videoSource248);

	const int num = 26;
	pid_t pid[num];
	for (int i=0; i<num; ++i) {
		pid[i] = fork();
		if (pid[i] == 0) {
			RSHandle rsHandle;
			rsOrionDecoderInit(&rsHandle);
			AddVideoSource(rsHandle, videoSource171);

			SetServerAddress(rsHandle, address, appKey, appSecret);
			SetFaceFilterPolicy(rsHandle, filter_policy);
			rsOrionDecoderStart(rsHandle);
			rsOrionDecoderInit(&rsHandle);

		}

	}
	return 0;
}
