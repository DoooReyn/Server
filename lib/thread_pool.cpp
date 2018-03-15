#include "thread_pool.h"

ThreadPool::ThreadPool(const string& nameArg)
	: m_mutex()
	, m_notEmpty(m_mutex)
	, m_notFull(m_mutex)
	, m_strName(nameArg)
	, m_maxQueueSize(0)
	, m_running(false)
{
}

ThreadPool::~ThreadPool()
{
	if(m_running)
	{
		Stop();
	}
}

void ThreadPool::Start(int32 numThreads)
{
	assert(m_threads.empty());
	m_running = true;
	m_threads.reserve(numThreads);
	for(int32 i = 0; i < numThreads; ++i)
	{
		char id[32] = {0};
		snprintf(id, sizeof(id), "%d", i + 1);
		m_threads.push_back(std::unique_ptr<Thread>(new Thread(std::bind(&ThreadPool::runInThread, this), m_strName + id)));
		m_threads[i]->Start();
	}
	if (numThreads == 0 && m_threadInitCallBack)
	{
		m_threadInitCallBack();
	}
}

void ThreadPool::Stop()
{
	{
		MutexLockGuard lock(m_mutex);
		m_running = false;
		m_notEmpty.NotifyAll();
	}

	std::for_each(m_threads.begin(), m_threads.end(), std::bind(&Thread::Join, _1));
}

size_t ThreadPool::QueueSize()const
{
	MutexLockGuard lock(m_mutex);
	return m_queue.size();
}

void ThreadPool::Run(const Task& task)
{
	if(m_threads.empty())
	{
		take();
	}
	else
	{
		MutexLockGuard lock(m_mutex);
		while (isFull())
		{
			m_notFull.Wait();
		}
		assert(!isFull());

		m_queue.push_back(std::move(task));
		m_notEmpty.Notify();
	}
}

bool ThreadPool::isFull()const
{
	m_mutex.AssertLocked();
	return m_maxQueueSize > 0 && m_queue.size() >= m_maxQueueSize;
}

ThreadPool::Task ThreadPool::take()
{
	MutexLockGuard lock(m_mutex);
	while(m_queue.empty() && m_running)
	{
		m_notEmpty.Wait();
	}

	Task task;
	if(!m_queue.empty())
	{
		task = m_queue.front();
		m_queue.pop_front();
		if(m_maxQueueSize > 0)
		{
			m_notFull.Notify();
		}
	}
	return task;
}

void ThreadPool::runInThread()
{
	try
	{
		if(m_threadInitCallBack)
		{
			m_threadInitCallBack();
		}
		while(m_running)
		{
			Task task(take());
			if(task)
			{
				task();
			}
		}
	}
	catch(const std::exception& ex)
	{
		fprintf(stderr, "exception caught in ThreadPool %s\n", m_strName.c_str());
		fprintf(stderr, "reason: %s\n", ex.what());
		abort();
	}
	catch(...)
	{
		fprintf(stderr, "unknown exception caught in ThreadPool %s\n", m_strName.c_str());
		throw;
	}
}
