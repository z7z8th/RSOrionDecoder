#ifndef __RS_TASK_QUEUE_H__
#define __RS_TASK_QUEUE_H__

#include <list>
#include <mutex>
#include <condition_variable>

namespace readsense {

template<typename T>
class CTaskQueue {
public:
	CTaskQueue(int size = -1)
		//: line_size_(size > 0 ? size : INT_MAX) {}
		: line_size_(size > 0 ? size : 200) {}
	CTaskQueue(const CTaskQueue<T> &);
	~CTaskQueue() {}

	void Push(const T &element);
	//void Push(const T &&element);
	T Pop();
	T Peek();
	void Clear();
	int Size();
	bool Empty();

private:
	int line_size_{ 0 };
	std::list<T> block_queue_;
	std::mutex mutex_;
	std::condition_variable_any full_notify_;
	std::condition_variable_any empty_notify_;
};

template<typename T>
CTaskQueue<T>::CTaskQueue(const CTaskQueue<T> &task_queue) {
	std::unique_lock<std::mutex> lock(mutex_);
	for (auto &element : task_queue.block_queue_) {
		block_queue_.push_back(element);
	}
}

template<typename T>
void CTaskQueue<T>::Push(const T &element) {
	std::unique_lock<std::mutex> lock(mutex_);
	full_notify_.wait(lock, [this]() { return this->block_queue_.size() < static_cast<size_t>(this->line_size_); });
	block_queue_.push_front(element);
	empty_notify_.notify_one();
}

//template<typename T>
//void CTaskQueue<T>::Push(const T &&element) {
//	std::unique_lock<std::mutex> lock(mutex_);
//	full_notify_.wait(lock, [this]() { return this->block_queue_.size() < this->line_size_; });
//	block_queue_.push_front(element);
//	empty_notify_.notify_one();
//}

template<typename T>
T CTaskQueue<T>::Pop() {
	std::unique_lock<std::mutex> lock(mutex_);
	empty_notify_.wait(lock, [this]() { return !this->block_queue_.empty(); });
	T ret = block_queue_.back();
	block_queue_.pop_back();
	full_notify_.notify_one();
	return std::move(ret);
}

template<typename T>
T CTaskQueue<T>::Peek() {
	std::unique_lock<std::mutex> lock(mutex_);
	empty_notify_.wait(lock, [this]() { return !this->block_queue_.empty(); });
	T ret = block_queue_.back();
	full_notify_.notify_one();
	return std::move(ret);
}

template<typename T>
void CTaskQueue<T>::Clear() {
	std::unique_lock<std::mutex> lock(mutex_);
	while (!block_queue_.empty()) {
		block_queue_.pop_back();
		full_notify_.notify_all();
	}
}

template<typename T>
int CTaskQueue<T>::Size() {
	std::unique_lock<std::mutex> lock(mutex_);
	return block_queue_.size();
}

template<typename T>
bool CTaskQueue<T>::Empty() {
	std::unique_lock<std::mutex> lock(mutex_);
	return (block_queue_.size() == 0);
}

} // namespace readsense


#endif // __TASK_QUEUE_H__
