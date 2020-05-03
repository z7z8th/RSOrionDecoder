#ifndef __RS_CHAINED_IO_IF_H__
#define __RS_CHAINED_IO_IF_H__
/* Chained IO Interface */

#include <memory>
#include "readsense/rs_runnable_alone_task_list.h"
#include "rs_avframe_cxx.h"

template <typename IODataT>
class RSChainedIOIf {
protected:
    virtual bool Output(IODataT event) = 0;
    virtual IODataT Input() = 0;
    virtual void ClearOutputed() = 0;
	virtual void FinishTaskChain(bool abort = false) = 0;
    virtual bool ShouldStop() = 0;
    virtual void SetAbortTaskChainFlag() = 0;
};

template <typename spIODataT = std::shared_ptr<RSAVEvent>>
class RSChainedIOTask :
    public RSChainedIOIf<spIODataT>,
    public readsense::CRunnableAloneTaskList<spIODataT>
{
    using IODataT = typename spIODataT::element_type;
    using Task = readsense::CRunnableAloneTaskList<spIODataT>;
protected:
    virtual bool Output(spIODataT event) {
        if (Task::sp_next_task_ != nullptr) {
            Task::sp_next_task_->Push(event);
        }
        return Task::sp_next_task_ != nullptr;
    }
    virtual spIODataT Input() {
        return Task::_Pop();
    }
    virtual void ClearOutputed() {
        if (Task::sp_next_task_ != nullptr) {
            Task::sp_next_task_->Clear();
        }
    }
	virtual void FinishTaskChain(bool abort = false) {
        spIODataT finish = std::make_shared<IODataT>();
        if (abort || flagAbortTaskChain) {
            ClearOutputed();
            finish->evt_ = IODataT::EVENT_ABORT;
        } else {
            finish->evt_ = IODataT::EVENT_EXIT;
        }
        
		Output(std::move(finish));
	}
    virtual bool ShouldStop() {
        return Task::stop_flag_;
    }
    virtual void SetAbortTaskChainFlag() {
        flagAbortTaskChain = true;
    }
    bool flagAbortTaskChain = false;
};

using RSChainedAVTask = RSChainedIOTask<spRSAVEvent>;

#endif // __RS_CHAINED_IO_IF_H__