#ifndef __EVENT_LOOP_THREAD_H__
#define __EVENT_LOOP_THREAD_H__

#include "condition.h"
#include "mutex.h"
#include "thread.h"

class EventLoop;
class EventLoopThread : private Nocopyable
{
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;

	EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), const string& name = string());
	~EventLoopThread();
	EventLoop* StartLoop();

private:
	void threadFunc();

	EventLoop* m_pLoop;
	bool m_bExiting;
	Thread m_thread;
	Mutex m_mutex;
	Condition m_cond;
	ThreadInitCallback m_callback;
};

#endif