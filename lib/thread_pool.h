#ifndef __THREADPOLL_H__
#define __THREADPOLL_H__
#include "type_define.h"
#include "nocopyable.h"
#include "mutex.h"
#include "condition.h"
#include "thread.h"

class ThreadPool : private Nocopyable
{
public:
	typedef std::function<void ()> Task;
	explicit ThreadPool(const string& nameArg = string("ThreadPool"));
	~ThreadPool();

	void SetMaxQueueSize(size_t maxSize)
	{
		m_maxQueueSize = maxSize;
	}

	void SetThreadInitCallBack(const Task& cb)
	{
		m_threadInitCallBack = cb;
	}

	void Start(int32 numThreads);
	void Stop();
	const string& name()const
	{
		return m_strName;
	}

	size_t QueueSize()const;

	void Run(const Task& f);

private:
	bool isFull() const;
	void runInThread();
	Task take();

	mutable Mutex m_mutex;
	Condition m_notEmpty;
	Condition m_notFull;
	string m_strName;
	Task m_threadInitCallBack;
	std::vector<std::unique_ptr<Thread> > m_threads;
	std::deque<Task> m_queue;
	size_t m_maxQueueSize;
	bool m_running;
};


#endif
