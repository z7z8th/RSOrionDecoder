#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "RSOrionDecoder.h"
#include "rs_fault.h"
#include <rs_proc_manager.h>

#ifdef _WIN32
#pragma comment(lib, "RSOrionDecoder.lib")
#endif

static RSProcManager& procMgr = RSProcManager::GetInstance();

int main()
{
    fprintf(stderr, "hello");
    register_fault_signals();
    procMgr.DebugByNoFork(true);
    std::string vSrcArray[] = {
        "/home/bob/Videos/test/test-1.mp4",
        "/home/bob/Videos/test/test-2.mkv",
        /*
        "/home/bob/Videos/test/1-muddy-puddle.mp4",
 		"/home/bob/Videos/test/2-mr-dinasour.mp4",
		"/home/bob/Videos/test/3-polly-parrot.mp4",
		"/home/bob/Videos/test/4-best-friend.mp4",

		"/home/bob/Videos/test/5-hide-and-seek.mp4",
		"/home/bob/Videos/test/6-the-playgroup.mp4",
		"/home/bob/Videos/test/7-mummy-pig-at-work.mp4",
		"/home/bob/Videos/test/8-camping.mp4",
        */
    };
    for (auto vsrc : vSrcArray) {
        procMgr.AddVideoSource(vsrc);
    }

    std::cout << "master process pid " << getpid() << std::endl;

    procMgr.StartChildProcs();
    procMgr.EventLoop();

    return 0;
}
