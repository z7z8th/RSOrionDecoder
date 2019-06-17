#ifndef __RS_RUNNABLE_ALONE_TASK_LIST_WITH_POST_H__
#define __RS_RUNNABLE_ALONE_TASK_LIST_WITH_POST_H__

#include <mutex>
#include <map>

#include "rs_runnable.h"
#include "rs_task_queue.h"

namespace readsense {

typedef std::function<void()> Task;

class CRunnableAloneTaskListWithPost : public readsense::CRunnable
{
public:
	CRunnableAloneTaskListWithPost() {}
	virtual ~CRunnableAloneTaskListWithPost() {}

	int GetTaskListSize() { return task_list_.Size(); }

	virtual void Start();
	virtual void Stop();

	//线程消息投递
	void Push(const Task&& func) { task_list_.Push(func); }
	void Push(const Task& func) { task_list_.Push(func); }

	//线程主逻辑
	virtual void Run();

	std::thread::id GetId()const { return id_; }

	//向指定线程投递任务
	static	bool PostTask(const std::thread::id& thread_id, const Task& func);

private:
	Task _Pop() { return std::move(task_list_.Pop()); }

	CRunnableAloneTaskListWithPost(const CRunnableAloneTaskListWithPost&) = delete;
	CRunnableAloneTaskListWithPost& operator=(const CRunnableAloneTaskListWithPost&) = delete;

private:
	readsense::CTaskQueue<Task>		task_list_;			//每个线程都有自己的任务队列
	std::thread::id			id_;

	static	std::mutex						map_mutex_;		//_runnable_map访问锁
	static	std::map<std::thread::id, \
		CRunnableAloneTaskListWithPost*>	runnable_map_;	//记录线程对象，支持线程间消息投递
};

std::mutex CRunnableAloneTaskListWithPost::map_mutex_;
std::map<std::thread::id, CRunnableAloneTaskListWithPost*> CRunnableAloneTaskListWithPost::runnable_map_;

void CRunnableAloneTaskListWithPost::Start() {
	//stop_flag_ = false;
	//if (!pthread_) {
	//	pthread_ = std::shared_ptr<std::thread>(new std::thread(std::bind(&CRunnableAloneTaskListWithPost::Run, this)));
	//}
	readsense::CRunnable::Start();

	Push([this]() {
		std::unique_lock<std::mutex> lock(map_mutex_);
		auto iter = runnable_map_.find(std::this_thread::get_id());
		if (iter == runnable_map_.end()) {
			runnable_map_[std::this_thread::get_id()] = this;
			id_ = std::this_thread::get_id();
		}
	});
}

void CRunnableAloneTaskListWithPost::Stop() {
	Push([this]() {
		{
			std::unique_lock<std::mutex> lock(map_mutex_);
			auto iter = runnable_map_.find(std::this_thread::get_id());
			if (iter != runnable_map_.end()) {
				runnable_map_.erase(iter);
			}
		}
		Push(nullptr);
		stop_flag_ = true;
	});
}

bool CRunnableAloneTaskListWithPost::PostTask( \
	const std::thread::id& thread_id, const Task& func) {
	std::unique_lock<std::mutex> lock(map_mutex_);
	auto iter = runnable_map_.find(thread_id);
	if (iter != runnable_map_.end()) {
		if (iter->second) {
			iter->second->Push(func);
			return true;
		}
	}
	return false;
}

void  CRunnableAloneTaskListWithPost::Run() {
	while (!stop_flag_) {
		auto t = _Pop();
		if (t) {
			t();

		}
		else {
			break;
		}
	}
}

}	// namespace readsense

#endif	// __RS_RUNNABLE_ALONE_TASK_LIST_WITH_POST_H__