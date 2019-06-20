#ifndef __RS_PATH_H__
#define __RS_PATH_H__
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>

#include "RSOrionDecoder.h"

using socket_t = int;

struct RSChildProcDesc {
    RSChildProcDesc() = default;
    bool AddVideoSource(const std::string &vsrc) {
        std::cout << "desc add vsrc " << vsrc << std::endl;
        if (vecVSrc.size() < MAX_VIDEO_PER_CHILD) {
            vecVSrc.push_back(vsrc);
            return true;
        }
        return false;
    }
    size_t   childIndex = -1;
    pid_t    pid = 0;
    RSHandle     rsOrionHandle = NULL;
    std::vector<std::string> vecVSrc;
    socket_t controlPipe = -1;
    size_t   MAX_VIDEO_PER_CHILD = 4;
};

class RSProcManager {
public:
    RSProcManager() = default;
    virtual ~RSProcManager() {
    }
    static RSProcManager& GetInstance() {
        static RSProcManager procMgr;
        return procMgr;
    }
    void AddVideoSource(const std::string &vsrc) {
        for (auto& desc : childMap) {
            if (desc.AddVideoSource(vsrc))
                return;
        }
        childMap.emplace_back();
        childMap.back().AddVideoSource(vsrc);
    }
    std::size_t ProcCnt() {
        return childMap.size();
    }
    void ChildProcRoutine(RSChildProcDesc &desc) {
            if (exiting)
                return;
            std::cout << "child process [" << desc.childIndex << "] pid " << getpid() << " starting" << std::endl;
            RSHandle rsHandle;
            rsOrionDecoderInit(&rsHandle);
            desc.rsOrionHandle = rsHandle;
            for (auto vsrc : desc.vecVSrc)
                ::AddVideoSource(rsHandle, vsrc);

            /*
			SetServerAddress(rsHandle, address, appKey, appSecret);
			SetFaceFilterPolicy(rsHandle, filter_policy);
			*/
            SetRtmpServer(rsHandle, "rtmp://127.0.0.1:1935", "/live/", desc.childIndex + 1);
            rsOrionDecoderStart(rsHandle);
            //rsOrionDecoderInit(&rsHandle);
            std::cout << "child process [" << desc.childIndex << "] pid " << getpid() << " started" << std::endl;
    }
    void StartChildProcs() {
        RegisterSignals();
        for (std::size_t i=0; i<childMap.size(); i++) {
        //for (auto& desc : childMap) {
            auto &desc(childMap[i]);
            desc.childIndex = i;
            desc.pid = fork();
            if (desc.pid == 0) {
                isChild = true;
                thisProcDesc = &desc;
                ChildProcRoutine(desc);
                return;
            }
        }
    }

    void ExitChildrenAndManager() {
        RSProcManager& procMgr = GetInstance();
        for (auto& desc : procMgr.childMap) {
            std::cout << "killing child " << desc.pid << " with SIGTERM"<<std::endl;
            kill(desc.pid, SIGTERM);
        }
        
        for (size_t i=0; i<procMgr.childMap.size() - sigCHLDCnt; i++) {
            int status;
            pid_t pid = waitpid(-1, &status, 0 /* WNOHANG */);
            std::cout << "child process (pid " << pid << ") exited with status " << std::hex << status << std::dec << std::endl;
        }

        ::exit(SIGTERM);
    }

    void ExitChildHimself() {
        RSProcManager& procMgr = GetInstance();
        rsOrionDecoderStop(procMgr.thisProcDesc->rsOrionHandle);
        ::exit(SIGTERM);
    }

    void EventLoop() {
        #warning dummy Manager Loop, should handle socket event from child
        while (!exiting) {
            if (sigCHLDCnt != 0)
                SigHandler_CHLD_nx();
            std::this_thread::sleep_for(std::chrono::milliseconds(1*1000));
        }
            
        if (isChild) {
            ExitChildHimself();
        } else {
            ExitChildrenAndManager();
        }
    }

    RSChildProcDesc* FindChildDesc(pid_t pid) {
        for (auto& desc : childMap) {
            if (desc.pid == pid)
                return &desc;
        }
        return nullptr;
    }
    void RestartChild(pid_t pid) {
        std::cout << "RestartChild for previously dead child process (pid " << pid << ")." << std::endl;
        RSChildProcDesc *desc = FindChildDesc(pid);
        if (!desc) {
            std::cout << "*** Not my child process (pid " << pid << ") ided." << std::endl
                      << "*** Manager process possibly corrupted." << std::endl;
            return;
        }
        
        desc->pid = fork();
        if (desc->pid == 0) {
            isChild = true;
            thisProcDesc = desc;
            ChildProcRoutine(*desc);
        }
    }

    void SigHandler_CHLD_nx() {
        int status;
        pid_t pid = waitpid(-1, &status, 0 /* WNOHANG */);
        std::cout << "waitpid() return pid " << pid << " exited with status " << std::hex << status << std::dec << std::endl;
        RestartChild(pid);
        --sigCHLDCnt;
    }
    static void SigHandler_CHLD(int sig, siginfo_t *info, void *ucontext) {
        RSProcManager& procMgr = GetInstance();
        std::cout << (procMgr.isChild ? "[child] " : "[parent] ") << __func__ << " sig " << sig << " si_code " << info->si_code
                  << " si_pid " << info->si_pid << " si_status " << info->si_status << std::endl;
        if (procMgr.exiting)
            return;
        if (procMgr.isChild)
            return;
        ++procMgr.sigCHLDCnt;
    }
    static void SigHandler_TERM(int sig, siginfo_t *info, void *ucontext) {
        RSProcManager& procMgr = GetInstance();
        procMgr.exiting = true;
        std::cout << (procMgr.isChild ? "[child] " : "[parent] ") << __func__ << " sig " << sig << " si_code " << info->si_code << std::endl;
    }
    int RegisterSignals() {
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO | SA_NODEFER;
        sa.sa_sigaction = SigHandler_CHLD;
        if (sigaction(SIGCHLD, &sa, NULL) < 0) {
            perror("handle SIGCHLD");
            return -1;
        }

        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = SigHandler_TERM;
        if (sigaction(SIGINT, &sa, NULL) < 0) {
            perror("handle SIGINT");
            return -1;
        }

        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = SigHandler_TERM;
        if (sigaction(SIGTERM, &sa, NULL) < 0) {
            perror("handle SIGTERM");
            return -1;
        }
        return 0;
    }

protected:
    std::vector<RSChildProcDesc> childMap;
    RSChildProcDesc* thisProcDesc;
    //static RSProcManager *procMgr;
    std::atomic<bool>  isChild { false };
    std::atomic<bool>  exiting { false };
    std::atomic<bool>  restartChild { false };
    std::atomic<int>   sigCHLDCnt { 0 };
};

#endif // __RS_PATH_H__