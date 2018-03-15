#include "timer_queue.h"
#include "logger.h"
#include "event_loop.h"

int32 createTimerfd()
{
	//创建一个定时器文件
	//CLOCK_MONOTONIC:CLOCK_REALTIME相反，它是以绝对时间为准，获取的时间为系统重启到现在的时间
	//CLOCK_REALTIME：相对时间，从1970.1.1到目前的时间。更改系统时间会更改获取的值
	//TFD_NONBLOCK（非阻塞），TFD_CLOEXEC（同O_CLOEXEC）
	int32 timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	//INFO("createTimerfd %d", timerfd);
	if (timerfd < 0)
	{
		ERROR("Failed in timerfd_create");
	}
	return timerfd;
}

void readTimerfd(int32 timerfd, Timestamp now)
{
	int64 howmany;
	ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
	//DEBUG("TimerQueue::handleRead() %lld at %s", howmany, now.ToString().c_str());
	if( n != sizeof(howmany))
	{
		ERROR("TimerQueue::handleRead() reads %lu bytes instead of 8", n);
	}
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
	int64 microSeconds = when.GetMicroSeconds() - Timestamp::Now().GetMicroSeconds();
	if(microSeconds < 100)
	{
		microSeconds = 100;
	}
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microSeconds / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>((microSeconds % Timestamp::kMicroSecondsPerSecond) *  1000);
	return ts;
}

void resetTimerfd(int32 timerfd, Timestamp expiration)
{
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue, sizeof newValue);
	bzero(&oldValue, sizeof oldValue);
	newValue.it_value = howMuchTimeFromNow(expiration);
	int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
	if (ret)
	{
		INFO("timerfd_settime()");
	}
}

TimerQueue::TimerQueue(EventLoop* loop)
	: m_pLoop(loop)
	, m_timerFd(createTimerfd())
	, m_timerFdChannel(loop, m_timerFd)
	, m_setTimer()
	, m_callingExpiredTimer(false)
{
	m_timerFdChannel.SetReadCallBack(std::bind(&TimerQueue::handleRead, this));
	m_timerFdChannel.EnableReading();
}

TimerQueue::~TimerQueue()
{
	m_timerFdChannel.DisableAll();
	m_timerFdChannel.Remove();
	::close(m_timerFd);
	for(TimerSet::iterator it = m_setTimer.begin(); it != m_setTimer.end(); ++it)
	{
		delete it->second;
	}
	m_setTimer.clear();
}
TimerId TimerQueue::AddTimer(const TimerCallback& cb, Timestamp when, double interval)
{
	TimerEx* timer = new TimerEx(cb, when, interval);
	m_pLoop->RunInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->Sequence());
}

void TimerQueue::Cancel(TimerId timerId)
{
	m_pLoop->RunInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}


bool TimerQueue::insert(TimerEx* timer)
{
	m_pLoop->AssertInLoopThread();
	assert(m_setTimer.size() == m_setActiveTimer.size());
	bool earliestChanged = false;
	Timestamp when = timer->Expiration();
	TimerSet::iterator it = m_setTimer.begin();
	if( it == m_setTimer.end() || when < it->first)
	{
		earliestChanged = true;
	}

	{
		std::pair<TimerSet::iterator, bool> result = m_setTimer.insert(TimerEntry(when, timer));
		assert(result.second);
	}

	{
		std::pair<ActiveTimerSet::iterator, bool> result = m_setActiveTimer.insert(ActiveTimer(timer, timer->Sequence()));
		assert(result.second);
	}

	assert(m_setTimer.size() == m_setActiveTimer.size());
	return earliestChanged;
}


void TimerQueue::addTimerInLoop(TimerEx* timer)
{
	m_pLoop->AssertInLoopThread();
	bool earliestChanged = insert(timer);
	if(earliestChanged)
	{
		resetTimerfd(m_timerFd, timer->Expiration());
	}
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
	m_pLoop->AssertInLoopThread();
	assert(m_setTimer.size() == m_setActiveTimer.size());
	ActiveTimer timer(timerId.m_timer, timerId.m_sequence);
	ActiveTimerSet::iterator it = m_setActiveTimer.find(timer);
	if (it != m_setActiveTimer.end())
	{
		size_t n = m_setTimer.erase(TimerEntry(it->first->Expiration(), it->first));
		assert(n == 1);
		delete it->first;
		m_setActiveTimer.erase(it);
	}
	else if (m_callingExpiredTimer)
	{
		m_cancelingTimer.insert(timer);
	}
	assert(m_setTimer.size() == m_setActiveTimer.size());
}

void TimerQueue::handleRead()
{
	m_pLoop->AssertInLoopThread();
	Timestamp now(Timestamp::Now());
	readTimerfd(m_timerFd, now);
	std::vector<TimerEntry> vecExpired = getExpired(now);
	m_callingExpiredTimer = true;
	m_cancelingTimer.clear();

	for (std::vector<TimerEntry>::iterator it = vecExpired.begin();
	        it != vecExpired.end(); ++it)
	{
		it->second->Run();
	}
	m_callingExpiredTimer = false;

	reset(vecExpired, now);

}


std::vector<TimerQueue::TimerEntry> TimerQueue::getExpired(Timestamp now)
{
	assert(m_setTimer.size() == m_setActiveTimer.size());
	std::vector<TimerEntry> vecExpired;
	TimerEntry sentry(now, reinterpret_cast<TimerEx*>(UINTPTR_MAX));
	TimerSet::iterator end = m_setTimer.lower_bound(sentry);
	assert(end == m_setTimer.end() || now < end->first);
	std::copy(m_setTimer.begin(), end, back_inserter(vecExpired));
	m_setTimer.erase(m_setTimer.begin(), end);

	for(std::vector<TimerEntry>::iterator it = vecExpired.begin();
	        it != vecExpired.end(); ++it)
	{
		ActiveTimer timer(it->second, it->second->Sequence());
		size_t n = m_setActiveTimer.erase(timer);
		assert(n == 1);
	}

	assert(m_setTimer.size() == m_setActiveTimer.size());
	return vecExpired;
}


void TimerQueue::reset(const std::vector<TimerEntry>& expired, Timestamp now)
{
	Timestamp nextExpire;
	for (std::vector<TimerEntry>::const_iterator it = expired.begin();
	        it != expired.end(); ++it)
	{
		ActiveTimer timer(it->second, it->second->Sequence());
		if (it->second->Repeat() && m_cancelingTimer.find(timer) == m_cancelingTimer.end())
		{
			it->second->Restart(now);
			insert(it->second);
		}
		else
		{
			delete it->second;
		}
	}

	if(!m_setTimer.empty())
	{
		nextExpire = m_setTimer.begin()->second->Expiration();
	}

	if(nextExpire.Valid())
	{
		resetTimerfd(m_timerFd, nextExpire);
	}
}
