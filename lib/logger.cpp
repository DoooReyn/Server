#include <fstream>
#include <stdarg.h>

#include "logger.h"



__thread char t_errnobuf[512];
const char* strerror_tl(int savedErrno)
{
	return strerror_r(savedErrno, t_errnobuf, sizeof (t_errnobuf));
}

enum LogType
{
	LogType_One,
	LogType_Two,
};

#define MSGBUF_MAX 1024

Logger* logger = NULL;

__thread char	g_log_file_name[256];
__thread int	g_log_line;
__thread char	g_class_name[32];
__thread uint32 g_class_id;
__thread char	g_name[32];

Logger& SetLoggerHead(Logger& logger, const char* filename, const uint32 line)
{
	const char* file = strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename;
	strncpy(g_log_file_name, file, sizeof(g_log_file_name));
	g_log_file_name[strlen(file)] = '\0';
	g_log_line = line;
	return logger;
}


void InitLogger(const string& filename, const char* Level, const string& servername)
{
	logger = new Logger(filename, servername);
	logger->setLevel(Level);
}

inline void FetchLoggerHead(std::string& str)
{
	char buf[256] = { 0 };
	snprintf(buf, 256, "%s:%d", g_log_file_name, g_log_line);
	str = buf;
}

#define getMessage(msg,msglen,pat)	\
do{\
	va_list ap;\
	memset(msg,0,msglen);\
	va_start(ap,pat);\
	vsnprintf(msg,msglen - 1,pat,ap);\
	va_end(ap);\
}while(false)


LoggerLevel Logger::_level = LoggerLevel_Debug;
bool Logger::_need_logfile = true;
bool Logger::_need_console = true;

Logger::Logger(const std::string& filename, const std::string& servername) : _filename(filename), _servername(servername)
{
	_ofile = NULL;
	_lastWriteTime = 0;
}

Logger::~Logger()
{
	delete _ofile;
	_ofile = NULL;
}

void Logger::debug(const char* pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg, MSGBUF_MAX, pattern);
	this->log(msg, LoggerLevel_Debug);
}

void Logger::info(const char* pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg, MSGBUF_MAX, pattern);
	this->log(msg, LoggerLevel_Info);
}

void Logger::warn(const char* pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg, MSGBUF_MAX, pattern);
	this->log(msg, LoggerLevel_Warn);
}

void Logger::error(const char* pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg, MSGBUF_MAX, pattern);
	this->log(msg, LoggerLevel_Error);
}

void Logger::log(const char* msg, LoggerLevel level)
{
	if(level < _level)
	{
		return;
	}

	_time.Now();
	char tbuf[64]  = {0};
	_time.Format(tbuf, 64);
	char lbuf[MSGBUF_MAX + 64];
	std::string head_str;
	FetchLoggerHead(head_str);
	std::string strLevel;
	switch(level)
	{
		case LoggerLevel_Debug:
		{
			strLevel = "DEBUG";
		}
		break;
		case LoggerLevel_Info:
		{
			strLevel = "INFO";
		}
		break;
		case LoggerLevel_Warn:
		{
			strLevel = "WARN";
		}
		break;
		case LoggerLevel_Error:
		{
			strLevel = "ERROR";
		}
		break;
		default:
			break;
	}

	snprintf(lbuf, MSGBUF_MAX + 64, "%s [%s][%s]:[%s]==>%s", tbuf, _servername.c_str(), strLevel.c_str(), head_str.c_str(), msg);
	_mutex.Lock();
	if(_need_console)
	{
		writeLogToConsole(lbuf, level);
	}
	if(_need_logfile)
	{
		writeLogToFile(lbuf);
	}
	_mutex.UnLock();
}

void Logger::writeLogToConsole(const char* msg, LoggerLevel level)
{
	char pre[12] = {0};
	char post[10] = "\033[0m";
	switch(level)
	{
		case LoggerLevel_Debug:
		{
			bzero(post, 10);
		}
		break;
		case LoggerLevel_Info:
		{
			strcpy(pre, "\033[32;1m");
		}
		break;
		case LoggerLevel_Warn:
		{
			strcpy(pre, "\033[33;1m");
		}
		break;
		case LoggerLevel_Error:
		{
			strcpy(pre, "\033[31;1m");
		}
		break;
		default:
			break;
	}
	printf("%s%s%s\n", pre, msg, post);
}

void Logger::writeLogToFile(const char* msg)
{
	uint32 sec = _time.Sec();
	if(!_ofile || sec / (60 * 60) != _lastWriteTime / (60 * 60))
	{
		if(_ofile != NULL)
		{
			delete _ofile;
			_ofile = NULL;
		}
		else if(_filename.empty())
		{
			return ;
		}

		_ofile = new std::ofstream;
		char buf[64];
		_time.Format(buf, 64, "%Y%m%d-%H");
		std::string of = _filename + "." + std::string(buf);
		_ofile->open(of.c_str(), std::ios::out | std::ios::app);
		if(!(*_ofile))
		{
			printf("打开日志文件错误%s\n", of.c_str());
			return;
		}
		std::ostringstream oss;
		oss.str("");
		oss << "ln --force -s " << of << " " << _filename;
		int ret = system(oss.str().c_str());
		if(ret == -1)
		{
			printf("链接文件错误%s\n", oss.str().c_str());
		}
	}
	(*_ofile) << msg << std::endl;
	_lastWriteTime = sec;

}

void Logger::setLevel(const char* level)
{
	if(strcmp(level, "debug") == 0)
	{
		_level = LoggerLevel_Debug;
	}
	else if(strcmp(level, "info") == 0)
	{
		_level = LoggerLevel_Info;
	}
	else if(strcmp(level, "warn") == 0)
	{
		_level = LoggerLevel_Warn;
	}
	else if(strcmp(level, "error") == 0)
	{
		_level = LoggerLevel_Error;
	}
}
