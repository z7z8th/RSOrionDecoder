#ifndef __RS_RUNNABLE_ALONE_TASK_LIST_H__
#define __RS_RUNNABLE_ALONE_TASK_LIST_H__

#include "rs_runnable.h"
#include "rs_task_queue.h"

namespace readsense {

//typedef std::function<void()> Task;
using Task = std::function<void()>;

template<typename T = Task>
class CRunnableAloneTaskList : public CRunnable
{
public:
	CRunnableAloneTaskList() {}
	virtual ~CRunnableAloneTaskList() {}

	int GetTaskListSize() { return task_list_.Size(); }

	// �߳���ϢͶ��
	void Push(const T&& t)	{ task_list_.Push(t); }
	void Push(const T& t)	{ task_list_.Push(t); }

	// �߳����߼�
	virtual void Run() = 0;
	virtual void SetSpNextTask(std::shared_ptr<CRunnableAloneTaskList<T>> \
							   sp_next_task);

protected:
	T _Pop() { return std::move(task_list_.Pop()); }
	T _Peek() { return std::move(task_list_.Peek()); }

	CRunnableAloneTaskList(const CRunnableAloneTaskList&) = delete;
	CRunnableAloneTaskList& operator=(const CRunnableAloneTaskList&) = delete;

protected:
	CTaskQueue<T>	task_list_;			// ÿ���̶߳����Լ����������
	std::shared_ptr<CRunnableAloneTaskList<T>> sp_next_task_{ nullptr };
};

template<typename T>
void CRunnableAloneTaskList<T>::SetSpNextTask(
	std::shared_ptr<CRunnableAloneTaskList<T>> sp_next_task) {
	sp_next_task_ = sp_next_task;
}

}		// namespace readsense

#endif		// __RS_RUNNABLE_ALONE_TASK_LIST_H__
