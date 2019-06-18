#ifndef __RS_RUNNABLE_TASK_LIST_H__
#define __RS_RUNNABLE_TASK_LIST_H__

#include "readsense/rs_runnable.h"
#include "readsense/rs_task_queue.h"

namespace readsense {

typedef std::function<void()> Task;

template<typename T = Task>
class CRunnableTaskList : public CRunnable
{
public:
	CRunnableTaskList() {}
	virtual ~CRunnableTaskList() {}

	int GetTaskListSrcSize() { return sp_task_list_src_->Size(); }
	int GetTaskListDstSize() { return sp_task_list_dst_->Size(); }

	void SetTaskListSrc(std::shared_ptr<CTaskQueue<T>> sp_src);
	void SetTaskListDst(std::shared_ptr<CTaskQueue<T>> sp_dst);

	void Push(const T&& t);
	void Push(const T& t);

	virtual void Run() = 0;

protected:
	T _Pop();

	CRunnableTaskList(const CRunnableTaskList&) = delete;
	CRunnableTaskList& operator=(const CRunnableTaskList&) = delete;

private:
	std::shared_ptr<CTaskQueue<T>>	sp_task_list_src_{ nullptr };
	std::shared_ptr<CTaskQueue<T>>	sp_task_list_dst_{ nullptr };
};

template<typename T = Task>
void CRunnableTaskList<T>::Push(const T&& t) { 
	if (sp_task_list_dst_ != nullptr) {
		sp_task_list_dst_->Push(t);
	}
}

template<typename T = Task>
void CRunnableTaskList<T>::Push(const T& t) { 
	if (sp_task_list_dst_ != nullptr) {
		sp_task_list_dst_->Push(t);
	}
}

template<typename T = Task>
T CRunnableTaskList<T>::Pop() { 
	if (sp_task_list_src_ != nullptr) {
		return std::move(sp_task_list_src_->Pop());
	}
}

template<typename T = Task>
void CRunnableTaskList<T>::SetTaskListSrc(
	std::shared_ptr<CTaskQueue<T>> sp_src) {
	sp_task_list_src_ = sp_src;
}

template<typename T = Task>
void CRunnableTaskList<T>::SetTaskListDst(
	std::shared_ptr<CTaskQueue<T>> sp_dst) {
	sp_task_list_dst_ = sp_dst;
}

}		// namespace readsense

#endif		// __RS_RUNNABLE_TASK_LIST_H__
