#ifndef __TIMER_QUEUE_H__
#define __TIMER_QUEUE_H__

#include "timer.h"
#include "channel.h"

class EventLoop;
class TimerQueue : private Nocopyable
{
public:
	explicit TimerQueue(EventLoop* loop);
	~TimerQueue();

	TimerId AddTimer(const TimerCallback& cb, Timestamp when, double interval);
	void Cancel(TimerId timerId);
private:
	typedef std::pair<Timestamp, TimerEx*> TimerEntry;
	typedef std::set<TimerEntry> TimerSet;
	typedef std::pair<TimerEx*, int64> ActiveTimer;
	typedef std::set<ActiveTimer> ActiveTimerSet;

	void addTimerInLoop(TimerEx* timer);
	void cancelInLoop(TimerId timerId);

	void handleRead();

	std::vector<TimerEntry> getExpired(Timestamp now);

	void reset(const std::vector<TimerEntry>& expired, Timestamp now);

	bool insert(TimerEx* timer);

	EventLoop* m_pLoop;
	const int32 m_timerFd;
	Channel m_timerFdChannel;
	TimerSet m_setTimer;
	ActiveTimerSet m_setActiveTimer;
	bool m_callingExpiredTimer;
	ActiveTimerSet m_cancelingTimer;
};

#endif
