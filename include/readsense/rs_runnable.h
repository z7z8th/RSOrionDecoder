#ifndef __RS_RUNNABLE_H__
#define __RS_RUNNABLE_H__

#include <thread>
#include <functional>

namespace readsense {


class CRunnable
{
public:
	CRunnable() : stop_flag_(false) {}
	virtual ~CRunnable() {}

	virtual void	Start();
	virtual void	Stop();
	virtual void	Join();
	virtual void	Run() = 0;		// main logic operations of thread
	static void		Sleep(int interval);
	bool			stop_flag() const;

protected:
	CRunnable(const CRunnable&) = delete;
	CRunnable& operator=(const CRunnable&) = delete;

protected:
	bool							stop_flag_;
	std::shared_ptr<std::thread>	pthread_;
};


void CRunnable::Start() {
	stop_flag_ = false;
	if (!pthread_) {
		pthread_ = std::shared_ptr<std::thread>(
			new std::thread(std::bind(&CRunnable::Run, this)));
	}
}

void CRunnable::Stop() {
	stop_flag_ = true;
}

void CRunnable::Join() {
	if (pthread_) {
		pthread_->join();
	}
}

bool CRunnable::stop_flag() const {
	return stop_flag_;
}

void CRunnable::Sleep(int interval) {
	std::this_thread::sleep_for(
		std::chrono::milliseconds(interval));
}


}	// namespace readsense


#endif		// __RS_RUNNABLE_H__