#ifndef __RS_CTIME_H__
#define __RS_CTIME_H__

#include <mutex>
//#include <ctime>
#include <time.h>
#include <chrono>
#include <string>

namespace readsense {

class CTime
{
public:
	CTime();
	~CTime();
	CTime(CTime const& t);
	
	std::string GetDateStr();
	int			GetDate();
	int			GetYearDay();
	int			GetMonthDay();
	int			GetWeekDay();
	int			GetMonth();
	int			GetYear();
	int			GetHour();
	int			GetMin();
	int			GetSec();
	long long	GetMsec();

	//return xxxx xx xx-xx:xx:xx
	std::string GetFormatTime();
	//return xxxx xx xx-xx:xx:xx
	bool GetFormatTime(char* res, int len);

	bool operator==(CTime const& t);
	bool operator>(CTime const& t);
	bool operator>=(CTime const& t);
	bool operator<(CTime const& t);
	bool operator<=(CTime const& t);

	void Now();

private:
	time_t      time_;
	std::tm     tm_;
	std::mutex  mutex_;
};

CTime::CTime() {
	Now();
}

CTime::~CTime() {

}

CTime::CTime(CTime const& t) {
	memcpy(this, &t, sizeof(*this));
}

std::string CTime::GetDateStr() {
	char tmp[32];
	snprintf(tmp, sizeof(tmp), "%04d%02d%02d", (tm_.tm_year + 1900),
		tm_.tm_mon + 1, tm_.tm_mday);
	return std::move(std::string(tmp));
}

int CTime::GetDate() {
	char tmp[32];
	snprintf(tmp, sizeof(tmp), "%04d%02d%02d", (tm_.tm_year + 1900),
		tm_.tm_mon + 1, tm_.tm_mday);
	return atoi(tmp);
}

int CTime::GetYearDay() {
	return tm_.tm_yday;
}

int CTime::GetMonthDay() {
	return tm_.tm_mday;
}

int CTime::GetWeekDay() {
	return tm_.tm_wday;
}

int CTime::GetMonth() {
	return tm_.tm_mon + 1;
}

int CTime::GetYear() {
	return tm_.tm_year + 1900;
}

int CTime::GetHour() {
	return tm_.tm_hour;
}

int CTime::GetMin() {
	return tm_.tm_min;
}

int CTime::GetSec() {
	return tm_.tm_sec;
}

long long CTime::GetMsec() {
	return time_;
}

std::string CTime::GetFormatTime() {
	char res[32] = { 0 };
	snprintf(res, sizeof(res), "%04d %02d %02d-%02d:%02d:%02d",
		(tm_.tm_year + 1900), tm_.tm_mon + 1, tm_.tm_mday,
		tm_.tm_hour, tm_.tm_min, tm_.tm_sec);
	return std::move(std::string(res));
}

bool CTime::GetFormatTime(char* res, int len) {
	if (len < 32) {
		return false;
	}
	snprintf(res, sizeof(res), "%04d %02d %02d-%02d:%02d:%02d",
		(tm_.tm_year + 1900), tm_.tm_mon + 1, tm_.tm_mday,
		tm_.tm_hour, tm_.tm_min, tm_.tm_sec);
	return true;
}

void CTime::Now() {
	time_ = std::chrono::time_point_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now()).time_since_epoch().count();

	time_t it = time_ / 1000;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		tm_ = *std::localtime(&it);		// unsafe, use localtime_s(windows)/localtime_r(linux)
	}
}

bool CTime::operator==(CTime const& t) {
	return time_ == t.time_;
}

bool CTime::operator>(CTime const& t) {
	return time_ > t.time_;
}

bool CTime::operator>=(CTime const& t) {
	return time_ >= t.time_;
}

bool CTime::operator<(CTime const& t) {
	return time_ < t.time_;
}

bool CTime::operator<=(CTime const& t) {
	return time_ <= t.time_;
}

}	// namespace readsense

#endif	// __RS_CTIME_H__
