#ifndef __TIMER_H__
#define __TIMER_H__

#include "type_define.h"
#include "nocopyable.h"
#include "atomic.h"
//时间定义类，封装对时间的操作
class Time
{
public:
	Time()
	{
		Now();
	}
	~Time() {}
	Time(const Time& rt)
	{
		m_ts = rt.m_ts;
		m_msec = rt.m_msec;
	}

	Time& operator= (const Time& rt)
	{
		m_ts = rt.m_ts;
		m_msec = rt.m_msec;
		return *this;
	}

	bool operator > (const Time& rt) const
	{
		return m_msec > rt.m_msec;
	}

	bool operator >= (const Time& rt) const
	{
		return m_msec >= rt.m_msec;
	}

	bool operator < (const Time& rt) const
	{
		return m_msec < rt.m_msec;
	}

	bool operator <= (const Time& rt) const
	{
		return m_msec <= rt.m_msec;
	}

	bool operator == (const Time& rt) const
	{
		return m_msec == rt.m_msec;
	}

	//delay为毫秒级别的
	void AddDelay(const uint64 delay);
	void Now();
	uint32 Sec() const;
	uint64 Msec() const;
	uint64 Usec() const;
	uint32  LeftUsec() const;
	uint64 Elapse(const Time& rt) const;
	int32 Format(char* buffer, size_t bufferlen, const char* format = "%Y-%m-%d %T");
private:
	friend class Clocker;
	struct timespec m_ts;
	uint64 m_msec;
	static int64 m_timesync;
};

//定时器封装
class Timer : public Nocopyable
{
public:
	explicit Timer(const uint64 cycle, bool bBegin = false, const uint64 delay = 0);
	explicit Timer(const uint64 cycle, bool bBegin,  Time& tv);

	void Reset(const uint64 cycle, const Time& cur);
	//void Current(const Time& cur);
	void Next(const Time& cur, const uint32 delay);
	void Next(const Time& cur);
	uint32 LeftSec(const Time& cur);
	uint64 LeftMSec(const Time& cur);
	bool operator()(const Time& cur);
	bool Empty();
	void Stop();
	void AddDelay(const Time& cur);
private:
	//计时周期 毫秒时长
	uint64 m_cycle;
	//定时器开关
	bool m_off;
	Time m_time;
};


class Clocker: public Nocopyable
{
public:
	// 以当前时间开始，interval作为闹钟周期，cycle作为闹钟点构造一个闹钟
	explicit Clocker(const int64 point, const int64 cycle);
	explicit Clocker(const int64 point, const int64 cycle, Time& ctv);
	void Next(const Time& cur);
	uint64 LeftSec(const Time& cur);
	bool operator()(const Time& cur);

private:
	void Delay();

private:
	const uint64 m_point;
	const uint64 m_cycle;
	int64 m_timesync;
	Time m_clock;
};

class Timestamp
{
public:
	Timestamp() : m_microSeconds(0)
	{
	}

	explicit Timestamp(int64 microSeconds) : m_microSeconds(microSeconds)
	{
	}

	void Swap(Timestamp& rhs)
	{
		std::swap(m_microSeconds, rhs.m_microSeconds);
	}

	string ToString() const;
	string ToForamttedString(bool showMicroseconds = true) const;

	bool Valid()const
	{
		return m_microSeconds > 0;
	}

	int64 GetMicroSeconds() const
	{
		return m_microSeconds;
	}

	time_t GetSeconds() const
	{
		return static_cast<time_t>(m_microSeconds / kMicroSecondsPerSecond);
	}

	static Timestamp Now();

	static Timestamp Invalid()
	{
		return Timestamp();
	}

	static Timestamp FromUnixTime(time_t t)
	{
		return FromUnixTime(t, 0);
	}

	static Timestamp FromUnixTime(time_t t, int microSeconds)
	{
		return Timestamp(static_cast<int64>(t) * kMicroSecondsPerSecond + microSeconds);
	}

	Timestamp& operator=(const Timestamp& rhs)
	{
		if(this != &rhs)
		{
			this->m_microSeconds = rhs.m_microSeconds;
		}
		return *this;
	}

public:
	static const int32 kMicroSecondsPerSecond = 1000000;
private:
	int64 m_microSeconds;

};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
	return lhs.GetMicroSeconds() < rhs.GetMicroSeconds();
}

inline bool operator<=(Timestamp lhs, Timestamp rhs)
{
	return lhs.GetMicroSeconds() <= rhs.GetMicroSeconds();
}

inline bool operator>(Timestamp lhs, Timestamp rhs)
{
	return lhs.GetMicroSeconds() > rhs.GetMicroSeconds();
}

inline bool operator>=(Timestamp lhs, Timestamp rhs)
{
	return lhs.GetMicroSeconds() >= rhs.GetMicroSeconds();
}


inline double TimeDiff(Timestamp& high, Timestamp& low)
{
	int64 diff = high.GetMicroSeconds() - low.GetMicroSeconds();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
	int64 delta = static_cast<int64>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.GetMicroSeconds() + delta);
}


typedef std::function<void()> TimerCallback;
class TimerEx : private Nocopyable
{
public:
	TimerEx(const TimerCallback& cb, Timestamp when, double interval)
		: m_callback(cb),
		  m_expiration(when),
		  m_interval(interval),
		  m_repeat(interval > 0.0),
		  m_sequence(m_numCreated.IncrementAndGet())
	{ }

	void Run()const
	{
		m_callback();
	}

	Timestamp Expiration() const
	{
		return m_expiration;
	}
	bool Repeat() const
	{
		return m_repeat;
	}
	int64 Sequence() const
	{
		return m_sequence;
	}

	void Restart(Timestamp now);

	static int64 NumCreated()
	{
		return m_numCreated.Get();
	}
private:
	const TimerCallback m_callback;
	Timestamp m_expiration;
	const double m_interval;
	const bool m_repeat;
	const int64 m_sequence;
	static AtomicInt64 m_numCreated;
};


class TimerId
{
public:
	TimerId() : m_timer(NULL), m_sequence(0) {}

	TimerId(TimerEx* timer, int64 seq) : m_timer(timer), m_sequence(seq) {}

	friend class TimerQueue;

private:
	TimerEx* m_timer;
	int64 m_sequence;
};


#endif
