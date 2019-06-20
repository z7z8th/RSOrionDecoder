#ifndef __RS_CHAINED_IO_IF_H__
#define __RS_CHAINED_IO_IF_H__
/* Chained IO Interface */

#include <memory>
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_avframe_cxx.h"

template <typename IODataT>
class RSChainedIOIf {
protected:
    virtual bool Output(IODataT frame) = 0;
    virtual IODataT Input() = 0;
    virtual void ClearOutputed() = 0;
	virtual void FinishTaskChain() = 0;
    virtual bool ShouldStop() = 0;

};

template <typename IODataT = std::shared_ptr<RSAVFramePacket>>
class RSChainedIOTask :
    public RSChainedIOIf<IODataT>,
    public readsense::CRunnableAloneTaskList<IODataT>
{
    using Task = readsense::CRunnableAloneTaskList<IODataT>;
protected:
    virtual bool Output(IODataT frame) {
        if (Task::sp_next_task_ != nullptr) {
            Task::sp_next_task_->Push(frame);
        }
        return Task::sp_next_task_ != nullptr;
    }
    virtual IODataT Input() {
        return Task::_Pop();
    }
    virtual void ClearOutputed() {
        if (Task::sp_next_task_ != nullptr) {
            Task::sp_next_task_->Clear();
        }
    }
	virtual void FinishTaskChain() {
        ClearOutputed();
		Output(IODataT());
	}
    virtual bool ShouldStop() {
        return Task::stop_flag_;
    }
};

using RSChainedAVTask = RSChainedIOTask<spRSAVFramePacket>;

#endif // __RS_CHAINED_IO_IF_H__