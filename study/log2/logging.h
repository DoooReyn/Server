#ifndef __LOGGING_H__
#define __LOGGING_H__
#include "timer.h"
#include "log_stream.h"

class Logging
{
public:
	enum LogLevel
	{
		DEBUG,
		INFO,
		WARN,
		ERROR,
		NUM_LOG_LEVELS,
	};

	class SourceFile
	{
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N]) : m_data(arr), m_size(N - 1)
		{
			const char* slash = strrchr(m_data, '/');
			if(slash)
			{
				m_data = slash + 1;
				m_size -= static_cast<int32>(m_data - arr);
			}
		}

		explicit SourceFile(const char* filename) : m_data(filename)
		{
			const char* slash = strrchr(filename, '/');
			if (slash)
			{
				m_data = slash + 1;
			}
			m_size = static_cast<int>(strlen(m_data));
		}

		const char* m_data;
		int m_size;
	};

	Logging(SourceFile file, int32 line);
	Logging(SourceFile file, int32 line, LogLevel level);
	Logging(SourceFile file, int32 line, LogLevel level, const char* func);
	Logging(SourceFile file, int32 line, bool toAbort);
	~Logging();
	LogStream& Stream() { return m_Impl.m_stream;}

	static LogLevel GetLogLevel();
	static void SetLogLevel(LogLevel level);

	typedef void(*OutPutFunc)(const char* msg, int32 len);
	typedef void(*FlushFunc)();
	static void SetOutPut(OutPutFunc);
	static void SetFlush(FlushFunc);



private:
	class Impl
	{
	public:
		typedef Logging::LogLevel LogLevel;
		Impl(LogLevel level, int32 savedErrno, const SourceFile& file, int line);
		void FormatTime();
		void Finish();

		Time m_time;
		LogStream m_stream;
		LogLevel m_level;
		int m_line;
		SourceFile m_basename;

	};

	Impl m_Impl;
};

extern Logging::LogLevel g_logLevel;
inline Logging::LogLevel Logging::GetLogLevel()
{
	return g_logLevel;
}

const char* strerror_tl(int savedErrno);

#define LOG_DEBUG if(Logging::GetLogLevel() <= Logging::DEBUG) \
					Logging(__FILE__,__LINE__,Logging::DEBUG,__func__).Stream()


#define LOG_INFO if(Logging::GetLogLevel() <= Logging::INFO) \
					Logging(__FILE__,__LINE__,Logging::INFO,__func__).Stream()

#define LOG_WARN  Logging(__FILE__, __LINE__, Logging::WARN).Stream()
#define LOG_ERROR Logging(__FILE__, __LINE__, Logging::ERROR).Stream()


#endif
