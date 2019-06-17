#ifndef __RS_LOG_H__
#define __RS_LOG_H__

#include <iostream>
#include <fstream>
#include <stdarg.h>

#include "rs_runnable_alone_task_list.h"
#include "rs_memory_pool.h"
#include "rs_time.h"
#include "rs_single.h"

namespace readsense {

enum LogLevel {
	LOG_DEBUG_LEVEL = 0x0001,
	LOG_INFO_LEVEL = 0x0003,
	LOG_WARN_LEVEL = 0x0007,
	LOG_ERROR_LEVEL = 0x000F,
	LOG_FATAL_LEVEL = 0x001F,
};

////template<typename T>
class CLog : public CRunnableAloneTaskList<char*>, public CSingle<CLog>
{
public:
	CLog();
	virtual ~CLog();
	virtual void Run();
	virtual void Stop();
	void SetLogName(const std::string& file_name);
	std::string GetLogName();

	void SetLogLevel(LogLevel level);
	LogLevel GetLogLevel();

	void LogDebug(const char* file, int line, const char* log...);
	void LogInfo(const char* file, int line, const char* log...);
	void LogWarn(const char* file, int line, const char* log...);
	void LogError(const char* file, int line, const char* log...);
	void LogFatal(const char* file, int line, const char* log...);

private:
	void _PushFormatLog(const char* file, int line, char* level,
						const char* log, va_list list);
	void _CheckDateFile();

private:
	CTime			time_;
	std::string     file_name_;
	std::fstream	log_file_;
	int				log_level_;
	int				cur_date_;
	CMemoryPool		pool_;
};

#define LOG_DEBUG(log, ...)			CLog::Instance().LogDebug(__FILE__, __LINE__, log, __VA_ARGS__);
#define LOG_INFO(log, ...)			CLog::Instance().LogInfo(__FILE__, __LINE__, log, __VA_ARGS__);
#define LOG_WARN(log, ...)			CLog::Instance().LogWarn(__FILE__, __LINE__, log, __VA_ARGS__);
#define LOG_ERROR(log, ...)			CLog::Instance().LogError(__FILE__, __LINE__, log, __VA_ARGS__);
#define LOG_FATAL(log, ...)			CLog::Instance().LogFatal(__FILE__, __LINE__, log, __VA_ARGS__);



const static int __log_buf_size = 1024;

//template<typename T>
CLog::CLog() 
	: log_level_(LOG_ERROR_LEVEL), 
	  cur_date_(0), pool_(__log_buf_size, 40) {

}

//template<typename T>
CLog::~CLog() {
	log_file_.close();
}

//template<typename T>
void CLog::Run() {
	while (!stop_flag_) {
		char* log = _Pop();
		if (log) {
			_CheckDateFile();
			if (log_file_.is_open()) {
				std::cout << log << std::endl;
				log_file_ << log << std::endl;
			}
			pool_.PoolLargeFree(log);
		}
	}
}

//template<typename T>
void CLog::Stop() {
	stop_flag_ = true;
	Push(nullptr);
}

//template<typename T>
void CLog::SetLogName(const std::string& file_name) {
	file_name_ = file_name;
}

//template<typename T>
std::string CLog::GetLogName() {
	return file_name_;
}

//template<typename T>
void CLog::SetLogLevel(LogLevel level) {
	log_level_ = level;
}

//template<typename T>
LogLevel CLog::GetLogLevel() {
	return (LogLevel)log_level_;
}

//template<typename T>
void CLog::LogDebug(const char* file, int line, const char* log...) {
	if (log_level_ & LOG_DEBUG_LEVEL) {
		va_list list;
		va_start(list, log);
		_PushFormatLog(file, line, "DEBUG", log, list);
		va_end(list);
	}
}

//template<typename T>
void CLog::LogInfo(const char* file, int line, const char* log...) {
	if (log_level_ & LOG_INFO_LEVEL) {
		va_list list;
		va_start(list, log);
		_PushFormatLog(file, line, "INFO", log, list);
		va_end(list);
	}
}

//template<typename T>
void CLog::LogWarn(const char* file, int line, const char* log...) {
	if (log_level_ & LOG_WARN_LEVEL) {
		va_list list;
		va_start(list, log);
		_PushFormatLog(file, line, "WARN", log, list);
		va_end(list);
	}
}

//template<typename T>
void CLog::LogError(const char* file, int line, const char* log...) {
	if (log_level_ & LOG_ERROR_LEVEL) {
		va_list list;
		va_start(list, log);
		_PushFormatLog(file, line, "ERROR", log, list);
		va_end(list);
	}
}

//template<typename T>
void CLog::LogFatal(const char* file, int line, const char* log...) {
	if (log_level_ & LOG_FATAL_LEVEL) {
		va_list list;
		va_start(list, log);
		_PushFormatLog(file, line, "FATAL", log, list);
		va_end(list);
	}
}

//template<typename T>
void CLog::_PushFormatLog(const char* file, int line, char* level, const char* log, va_list list) {
	time_.Now();

	char* time = pool_.PoolMalloc<char>(32);
	time_.GetFormatTime(time, 32);
	char* log_str = pool_.PoolLargeMalloc<char>();
	int curlen = snprintf(log_str, __log_buf_size, "[%s:%s-%s:%d] ", time, level, file, line);
	pool_.PoolFree(time, 32);

	if (curlen < 0) {
		strncpy(log_str, "...Log format error!", 32);

	}
	else {
		vsnprintf(log_str + curlen, __log_buf_size - curlen, log, list);
	}
	Push(log_str);
}

//template<typename T>
void CLog::_CheckDateFile() {
	if (cur_date_ != time_.GetDate()) {
		cur_date_ = time_.GetDate();
		if (log_file_.is_open()) {
			log_file_.close();
		}

		std::string file_name = file_name_.append(".");
		file_name.append(time_.GetDateStr());
		file_name.append(".log");

		log_file_.open(file_name.c_str(), std::ios::app | std::ios::out);
	}
}

}	// namespace readsense

#endif	// __RS_LOG_H__
