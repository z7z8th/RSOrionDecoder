#ifndef H_THREAD_H
#define H_THREAD_H

#include "hdef.h"
#include "hplatform.h"
#include <thread>
#include <atomic>
#include <chrono>

/*
#ifdef _MSC_VER
inline uint32 getpid(){
    return GetCurrentProcessId();
}
#endif
*/

inline uint32 gettid(){
#ifdef _MSC_VER
    return GetCurrentThreadId();
#else
    //return static_cast<uint32>(pthread_self());
    return std::hash<std::thread::id>()(std::this_thread::get_id());
#endif
}

/************************************************
 * HThread
 * Status: STOP,RUNNING,PAUSE
 * Control: start,stop,pause,resume
 * first-level virtual: doTask
 * second-level virtual: run
************************************************/
class HThread{
public:
    HThread() {
        status = INIT;
        status_switch = false;
        sleep_policy = YIELD;
        sleep_ms = 0;
    }

    virtual ~HThread() {

    }

    virtual int start() {
        if (status == STOP || status == INIT) {
            switchStatus(RUNNING);
            dotask_cnt = 0;

            thread = std::thread([this]{
                doPrepare();
                run();
                doFinish();
            });
        }
        return 0;
    }

    virtual int stop() {
        if (status != STOP && status != INIT) {
            switchStatus(STOP);
            thread.join(); // wait thread exit
        }
        return 0;
    }

    virtual int pause() {
        if (status == RUNNING) {
            switchStatus(PAUSE);
        }
        return 0;
    }

    virtual int resume() {
        if (status == PAUSE) {
            switchStatus(RUNNING);
        }
        return 0;
    }

    virtual void run() {
        while (status != STOP && status != INIT) {
            while (status == PAUSE) {
                std::this_thread::yield();
            }

            doTask();
            dotask_cnt++;

            sleep();
        }
    }

    virtual void doPrepare() {}
    virtual void doTask() {}
    virtual void doFinish() {}

    std::thread thread;
	enum Status {
		STOP,
		RUNNING,
		PAUSE,
		INIT
    };
    std::atomic<Status> status;
    uint32 dotask_cnt;

    enum SleepPolicy {
        YIELD,
        SLEEP_FOR,
        SLEEP_UNTIL,
        NO_SLEEP,
    };

    void setSleepPolicy(SleepPolicy policy, long long ms = 0) {
        status_switch = true;
        sleep_policy = policy;
        sleep_ms = ms;
    }

protected:
    void switchStatus(Status stat) {
        status_switch = true;
        status = stat;
    }

    void sleep() {
        switch (sleep_policy) {
        case YIELD: 
            std::this_thread::yield();  
            break;
        case SLEEP_FOR: 
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));    
            break;
        case SLEEP_UNTIL: {
            if (status_switch) {
                status_switch = false;
                base_tp = std::chrono::system_clock::now();
            }
            base_tp += std::chrono::milliseconds(sleep_ms);
            std::this_thread::sleep_until(base_tp); 
        }
            break;
        default:    // donothing, go all out.
            break;
        }
    }

    std::atomic<bool> status_switch;
    SleepPolicy sleep_policy;
    std::chrono::system_clock::time_point base_tp;   // for SLEEP_UNTIL
    long long sleep_ms;
};

#endif // H_THREAD_H
