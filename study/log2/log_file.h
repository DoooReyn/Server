#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__
#include "nocopyable.h"
#include "mutex.h"

class AppendFile;
class LogFile : public Nocopyable
{
public:
	LogFile(const string& basename, size_t roll_size, bool thread_safe = true, int32 flush_interval = 3, int32 check_everyN = 1024);
	~LogFile();
	void append(const char* logline, int32 len);
	void flush();
	bool RollFile();
private:

	void append_unlocked(const char* logline, int32 len);
	static string getLogFileName(const string& basename, time_t* now);

	const string m_basename;
	const size_t m_roll_size;
	const int32 m_checkEveryN;
	const int32 m_flushInterval;

	int32 m_count;
	time_t m_last_roll;
	time_t m_last_flush;
	time_t m_start_period;
	std::auto_ptr<AppendFile> m_file;
	std::auto_ptr<Mutex> m_mutex;
	const static int32 kRollPerSeconds = 60 * 60 * 24;
};


#endif
