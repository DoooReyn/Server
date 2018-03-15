#include "event_loop_thread_pool.h"
#include "event_loop.h"
#include "event_loop_thread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, const string& name)
	: m_baseLoop(loop)
	, m_strName(name)
	, m_bStarted(false)
	, m_numThreads(0)
	, m_next(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::Start(const ThreadInitCallback& cb /*= ThreadInitCallback()*/)
{
	assert(!m_bStarted);
	m_baseLoop->AssertInLoopThread();
	m_bStarted = true;

	for (int32 i = 0; i < m_numThreads; ++i)
	{
		char buf[m_strName.size() + 32] = { 0 };
		snprintf(buf, sizeof(buf), "%s%d", m_strName.c_str(), i);

		m_threads.push_back(std::unique_ptr<EventLoopThread>(new EventLoopThread(cb, buf)));
		m_loops.push_back(m_threads.back()->StartLoop());
	}
	if (m_numThreads == 0 && cb)
	{
		cb(m_baseLoop);
	}
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
	m_baseLoop->AssertInLoopThread();
	assert(m_bStarted);
	EventLoop* loop = m_baseLoop;
	if (!m_loops.empty())
	{
		loop = m_loops[m_next];
		++m_next;
		if (static_cast<size_t>(m_next) >= m_loops.size())
		{
			m_next = 0;
		}
	}
	return loop;
}

EventLoop* EventLoopThreadPool::GetLoopForHash(size_t hashCode)
{
	m_baseLoop->AssertInLoopThread();
	EventLoop* loop = m_baseLoop;
	if (!m_loops.empty())
	{
		loop = m_loops[hashCode % m_loops.size()];
	}
	return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::GetAllLoops()
{
	m_baseLoop->AssertInLoopThread();
	assert(m_bStarted);
	if (m_loops.empty())
	{
		return std::vector<EventLoop*>(1, m_baseLoop);
	}
	else
	{
		return m_loops;
	}
}
