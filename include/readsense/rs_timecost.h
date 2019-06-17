#ifndef __RS_TIME_H__
#define __RS_TIME_H__

#include <chrono>
#include <ratio>

namespace readsense {

class TimeCost
{
public:
	int start();
	int stop();

private:
	std::chrono::duration<double, std::milli> time_;
	std::chrono::steady_clock::time_point tp_;
};

int TimeCost::start()
{
	tp_ = std::chrono::steady_clock::now();
	return 0;
}

int TimeCost::stop()
{
	auto time_span = std::chrono::steady_clock::now() - tp_;
	time_ = std::chrono::duration_cast<std::chrono::milliseconds>(time_span);

	return time_.count();
}

}	// readsense

#endif	// end of __RS_TIME_H__
