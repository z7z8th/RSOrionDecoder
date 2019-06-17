#include <string>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include "RSOrionDecoder.h"

#ifdef _WIN32
#pragma comment(lib, "RSOrionDecoder.lib")
#endif

int main()
{
	//std::string videoSource171("rtsp://10.1.12.171:554/media/live/1/1");
	//std::string videoSource171("/home/bob/src/ffmpeg/test.mp4");
	std::string vSrcArray[] = {
		//"/home/bob/Videos/test/ffmpeg/test.mp4",
		"/home/bob/Videos/test/1-muddy-puddle.mp4",
		"/home/bob/Videos/test/2-mr-dinasour.mp4",
		"/home/bob/Videos/test/3-polly-parrot.mp4",
		"/home/bob/Videos/test/4-best-friend.mp4",
		"/home/bob/Videos/test/5-hide-and-seek.mp4",
		"/home/bob/Videos/test/6-the-playgroup.mp4",
		"/home/bob/Videos/test/7-mummy-pig-at-work.mp4",
		"/home/bob/Videos/test/8-camping.mp4",
	};
/* 	std::string videoSource234("rtsp://10.1.12.234:554/media/live/1/1");
	std::string videoSource67("rtsp://10.1.12.67:554/media/live/1/1");
	std::string videoSource17("rtsp://10.1.12.17:554/media/live/1/1");
	std::string videoSource16("rtsp://10.1.12.16:554/media/live/1/1");
	std::string videoSource18("rtsp://10.1.12.18:554/media/live/1/1");
	std::string videoSource247("rtsp://10.1.12.247:554/media/live/1/1");
	std::string videoSource248("rtsp://10.1.12.248:554/media/live/1/1");
	std::string videoSource235("rtsp://10.1.12.235:554/media/live/1/1");
	std::string videoSource151("rtsp://10.1.12.151:554/media/live/1/1");
	std::string videoSourceMp4("/home/xiaobai/src/ffmpeg/test.mp4"); */

/* 	std::string address = "39.106.21.118";
	const std::string appKey = "29ae8ff0386f4810";
	const std::string appSecret = "39a81d0d7d0e4948"; */

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

	std::cout<<"master process" << std::endl;

	const bool NO_FORK_FOR_DEBUG = false;
	int procCnt = 2; // 2 process
	int vCntPerProc = 4;  // 4 thread each
	pid_t pid[procCnt];

	if (NO_FORK_FOR_DEBUG) {
		procCnt = 1;
		vCntPerProc = 1;
	}

	for (int iproc=0; iproc<procCnt; ++iproc) {
		pid[iproc] = 0;
		if (!NO_FORK_FOR_DEBUG)
			pid[iproc] = fork();
		if (pid[iproc] == 0) {
			std::cout<<"child process [" << iproc << "] pid " << getpid() << std::endl;
			RSHandle rsHandle;
			rsOrionDecoderInit(&rsHandle);
			for (int ivsrc=0; ivsrc<vCntPerProc; ivsrc++)
				AddVideoSource(rsHandle, vSrcArray[iproc * vCntPerProc + ivsrc]);

			/*
			SetServerAddress(rsHandle, address, appKey, appSecret);
			*/
			//SetFaceFilterPolicy(rsHandle, filter_policy);
			SetRtmpServer(rsHandle, "rtmp://127.0.0.1:1935", "/live/", iproc+1);
			rsOrionDecoderStart(rsHandle);
			//rsOrionDecoderInit(&rsHandle);
			std::cout<<"child process [" << iproc << "] pid " << getpid()  << "done" << std::endl;
			return 0;
		}
	}

	for (int iproc=0; iproc<procCnt; ++iproc) {
		int status;
		int pid = wait(&status);
		std::cout << "child " << pid << " exited with " << status <<std::endl; 
	}
	return 0;
}
