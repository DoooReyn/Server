#include "event_loop.h"
#include "poller.h"
#include "timer_queue.h"
#include "logger.h"

__thread EventLoop* t_loopInThisThread = nullptr;
const int kPollTimeMs = 10000;


int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	//INFO("createEventfd %d", evtfd);
	if (evtfd < 0)
	{
		ERROR("Failed in eventfd");
		abort();
	}
	return evtfd;
}

EventLoop* EventLoop::GetEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	: m_looping(false)
	, m_quit(false)
	, m_eventHanding(false)
	, m_callPendingFunctor(false)
	, m_iteration(0)
	, m_threadId(CurrentThread::Tid())
	, m_poller(Poller::NewDefaultPoller(this))
	, m_timerQueue(new TimerQueue(this))
	, m_wakeupFd(createEventfd())
	, m_wakeupChannel(new Channel(this, m_wakeupFd))
	, m_currentActiveChannel(nullptr)
{
	//DEBUG("EventLoop created : %p in thread :%d", this, m_threadId);
	if(t_loopInThisThread)
	{
		ERROR("Another EventLoop %p exists in this thread :%d ", this, m_threadId);
		abort();
	}
	else
	{
		t_loopInThisThread = this;
	}
	m_wakeupChannel->SetReadCallBack(std::bind(&EventLoop::handleRead, this));
	m_wakeupChannel->EnableReading();
}

EventLoop::~EventLoop()
{
	//DEBUG("EventLoop %p of thread : %d destructs in thread : %d", this, m_threadId, CurrentThread::Tid());
	m_wakeupChannel->DisableAll();
	m_wakeupChannel->Remove();
	::close(m_wakeupFd);
	t_loopInThisThread = nullptr;

}

void EventLoop::Loop()
{
	assert(!m_looping);
	AssertInLoopThread();
	m_looping = true;
	//DEBUG("EvnetLoop %p start looping", this);

	while(!m_quit)
	{
		m_vecActiveChannel.clear();
		m_pollReturnTime = m_poller->Poll(kPollTimeMs, &m_vecActiveChannel);
		++m_iteration;
		//屏蔽日志
		//if(Logger::getLoggerLevel() < LoggerLevel_Error)
		//{
		//	printActiveChannel();
		//}
		m_eventHanding = true;
		for(vecChannel::iterator it = m_vecActiveChannel.begin();
		        it != m_vecActiveChannel.end(); ++it)
		{
			m_currentActiveChannel = *it;
			m_currentActiveChannel->HandleEvent(m_pollReturnTime);
		}
		m_currentActiveChannel = nullptr;
		m_eventHanding = false;
		doPendingFunctor();

	}
	//DEBUG("EvnetLoop %p stop looping", this);
	m_looping = false;
}

void EventLoop::Quit()
{
	m_quit = true;;

	if(!IsInLoopThread())
	{
		Wakeup();
	}
}

void EventLoop::RunInLoop(const Functor& cb)
{
	//DEBUG("Threadid:%d Tid:%d", m_threadId, CurrentThread::Tid());
	if(IsInLoopThread())
	{
		cb();
	}
	else
	{
		QueueInLoop(cb);
	}
}

void EventLoop::QueueInLoop(const Functor& cb)
{
	{
		MutexLockGuard lock(m_mutex);
		m_pendingFunctor.push_back(cb);
	}

	if(!IsInLoopThread() || m_callPendingFunctor)
	{
		Wakeup();
	}
}

size_t EventLoop::QueueSize() const
{
	MutexLockGuard lock(m_mutex);
	return m_pendingFunctor.size();
}

TimerId EventLoop::RunAt(const Timestamp& time, const TimerCallback& cb)
{
	return m_timerQueue->AddTimer(cb, time, 0.0);
}

TimerId EventLoop::RunAfter(double delay, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::Now(), delay));
	return RunAt(time, cb);
}

TimerId EventLoop::RunEvery(double interval, const TimerCallback& cb)
{
	Timestamp time(addTime(Timestamp::Now(), interval));
	return m_timerQueue->AddTimer(cb, time, interval);
}

TimerId EventLoop::RunAtDay(int32 hour, int32 min, int32 sec, const TimerCallback& cb)
{
	Timestamp time(Timestamp::FromUnixTime(hour, min, sec));
	//cout << time.GetSeconds() << endl;
	if (time < Timestamp::Now())
	{
		time.AddDelay(ONE_DAY);
	}
	//cout << time.GetSeconds() << endl;

	return m_timerQueue->AddTimer(cb, time, ONE_DAY);
}


void EventLoop::Cancel(TimerId timerId)
{
	return m_timerQueue->Cancel(timerId);
}

void EventLoop::UpdateChannel(Channel* channel)
{
	EventLoop* pLoop = channel->OwnerLoop();
	assert(pLoop == this);
	AssertInLoopThread();
	m_poller->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel)
{
	EventLoop* pLoop = channel->OwnerLoop();
	assert(pLoop == this);
	AssertInLoopThread();
	if (m_eventHanding)
	{
		assert(m_currentActiveChannel == channel ||
		       std::find(m_vecActiveChannel.begin(), m_vecActiveChannel.end(), channel) == m_vecActiveChannel.end());
	}
	m_poller->RemoveChannel(channel);
}

bool EventLoop::HasChannel(Channel* channel)
{
	EventLoop* pLoop = channel->OwnerLoop();
	assert(pLoop == this);
	AssertInLoopThread();
	return m_poller->HasChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
	ERROR("EventLoop::abortNotInLoopThread - EventLoop %p was created in threadId :%d current thread id :%d", this, m_threadId, CurrentThread::Tid());
	abort();
}

void EventLoop::Wakeup()
{
	int64 one = 1;
	ssize_t n = ::write(m_wakeupFd, &one, sizeof(one));
	if (n != sizeof(one))
	{
		ERROR("EventLoop::wakeup() writes :%ld bytes instead of 8", n);
	}
	//WARN("EventLoop Wakeup Threadid:%d Tid:%d",m_threadId,CurrentThread::Tid());
}


void EventLoop::handleRead()
{
	int64 one = 1;
	ssize_t n = ::read(m_wakeupFd, &one, sizeof(one));
	if (n != sizeof(one))
	{
		ERROR("EventLoop::wakeup() writes :%ld bytes instead of 8", n);
	}
	//WARN("EventLoop handleRead Threadid:%d Tid:%d",m_threadId,CurrentThread::Tid());
}

void EventLoop::doPendingFunctor()
{
	if (m_pendingFunctor.size() == 0)
	{
		return ;
	}

	std::vector<Functor> functors;
	m_callPendingFunctor = true;

	{
		MutexLockGuard lock(m_mutex);
		functors.swap(m_pendingFunctor);
	}

	for (size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}
	m_callPendingFunctor = false;
}

void EventLoop::printActiveChannel() const
{
	for (vecChannel::const_iterator it = m_vecActiveChannel.begin();
	        it != m_vecActiveChannel.end(); ++it)
	{
		const Channel* ch = *it;
		DEBUG ("{%s}", ch->RevertsToString().c_str());
	}
}

