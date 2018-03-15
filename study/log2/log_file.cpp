#include "log_file.h"
#include "file_util.h"

LogFile::LogFile(const string& basename, size_t roll_size, bool thread_safe, int32 flush_interval, int32 check_everyN)
	: m_basename(basename)
	, m_roll_size(roll_size)
	, m_checkEveryN(check_everyN)
	, m_flushInterval(flush_interval)
	, m_count(0)
	, m_last_roll(0)
	, m_last_flush(0)
	, m_start_period(0)
	, m_mutex(thread_safe ? new Mutex : NULL)
{
	assert(basename.find('/') == string::npos);
	RollFile();
}

LogFile::~LogFile()
{

}

void LogFile::append(const char* logline, int32 len)
{
	if (m_mutex.get())
	{
		MutexLockGuard lock(*m_mutex);
		append_unlocked(logline, len);
	}
	else
	{
		append_unlocked(logline, len);
	}
}

void LogFile::flush()
{
	if(m_mutex.get())
	{
		MutexLockGuard lock(*m_mutex);
		m_file->flush();
	}
	else
	{
		m_file->flush();
	}
}

bool LogFile::RollFile()
{
	time_t now = 0;
	string filename = getLogFileName(m_basename, &now);
	time_t start = now / kRollPerSeconds * kRollPerSeconds;

	if (now > m_last_roll)
	{
		m_last_roll = now;
		m_last_flush = now;
		m_start_period = start;

		m_file.reset(new AppendFile(filename));
		return true;
	}
	return false;
}

void LogFile::append_unlocked(const char* logline, int32 len)
{
	m_file->append(logline, len);
	if (m_file->WrittenBytes() > m_roll_size)
	{
		RollFile();
	}
	else
	{
		++m_count;
		if (m_count < m_checkEveryN)
		{
			return;
		}

		m_count = 0;
		time_t now = ::time(NULL);
		time_t thisPeriod = now / kRollPerSeconds * kRollPerSeconds;

		if (thisPeriod != m_start_period)
		{
			RollFile();
		}
		else if(now - m_last_flush > m_flushInterval)
		{
			m_last_flush = now;
			m_file->flush();
		}
	}
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
	string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[32] = {0};
	struct tm timeTm;
	*now = time(NULL);
	gmtime_r(now, &timeTm);
	strftime(timebuf, sizeof(timebuf), "_%Y%m%d-%H%M%S", &timeTm);
	filename += timebuf;
	filename += ".log";
	return filename;
}
