#ifndef __LOGGER_H__
#define __LOGGER_H__
#include "type_define.h"
#include "timer.h"
#include "mutex.h"

enum LoggerLevel
{
	LoggerLevel_Debug,
	LoggerLevel_Info,
	LoggerLevel_Warn,
	LoggerLevel_Error,
};

class Logger
{
public:
	Logger(const std::string& filename = "Server", const std::string& servername = "");
	~Logger();

	void debug(const char* pattern, ...) __attribute__((format(printf, 2, 3))); //省略形参(隐藏this) 使用printf 格式检查输出类型
	void info(const char* pattern, ...) __attribute__((format(printf, 2, 3)));
	void warn(const char* pattern, ...) __attribute__((format(printf, 2, 3)));
	void error(const char* pattern, ...) __attribute__((format(printf, 2, 3)));

	void removeConsole()
	{
		m_need_console = false;
	}
	void remvoeLogFile()
	{
		m_need_logfile = false;
	}

	void setLevel(const char* level);
	void setLogFileName(const std::string& name)
	{
		m_filename = name;
	}

	static LoggerLevel getLoggerLevel()
	{
		return m_level;
	}
private:
	void log(const char* msg, LoggerLevel level);
	void writeLogToConsole(const char* msg, LoggerLevel level);
	void writeLogToFile(const char* msg);
private:

	std::string m_filename;
	std::string m_servername;
	std::ofstream* m_ofile;
	uint32 m_lastWriteTime;

	Mutex m_mutex;
	Time m_time;
	static LoggerLevel m_level;
	static bool m_need_console;
	static bool m_need_logfile;
};

Logger& SetLoggerHead(Logger& logger, const char* filename, const uint32 line);
void SetLoggerHead(const char* class_name, const uint32& id, const char* name, const char* filename, const uint32& line);
void InitLogger(const string& filename, const char* Level, const string& servername = "");
const char* strerror_tl(int savedErrno);

extern Logger* logger;

#define DEBUG	SetLoggerHead( *logger, __FILE__, __LINE__ ).debug
#define INFO	SetLoggerHead( *logger, __FILE__, __LINE__ ).info
#define WARN	SetLoggerHead( *logger, __FILE__, __LINE__ ).warn
#define ERROR	SetLoggerHead( *logger, __FILE__, __LINE__ ).error
#define SETLOGLV(level) SetLoggerHead( *logger, __FILE__, __LINE__ ).setLevel(level);
#endif
