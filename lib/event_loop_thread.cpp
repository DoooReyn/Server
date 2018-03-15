#include "event_loop_thread.h"
#include "event_loop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb /*= ThreadInitCallback()*/, const string& name /*= string()*/)
	: m_pLoop(NULL)
	, m_bExiting(false)
	, m_thread(std::bind(&EventLoopThread::threadFunc, this), name)
	, m_mutex()
	, m_cond(m_mutex)
	, m_callback(cb)

{

}

EventLoopThread::~EventLoopThread()
{
	m_bExiting = true;
	if (NULL != m_pLoop)
	{
		m_pLoop->Quit();
		m_thread.Join();
	}
}

EventLoop* EventLoopThread::StartLoop()
{
	assert(!m_thread.Started());
	m_thread.Start();
	{
		MutexLockGuard lock(m_mutex);
		while (m_pLoop == NULL)
		{
			m_cond.Wait();
		}
	}
	return m_pLoop;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;
	if (m_callback)
	{
		m_callback(&loop);
	}

	{
		MutexLockGuard lock(m_mutex);
		m_pLoop = &loop;
		m_cond.Notify();
	}

	loop.Loop();

	m_pLoop = NULL;
}
