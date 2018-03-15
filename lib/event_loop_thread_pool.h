#ifndef __EVENT_LOOP_THREAD_POOL_H__
#define __EVENT_LOOP_THREAD_POOL_H__

#include "nocopyable.h"
#include "type_define.h"

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : private Nocopyable
{
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;
	EventLoopThreadPool(EventLoop* loop, const string& name);
	~EventLoopThreadPool();

	void SetThreadNum(int32 numThreads) { m_numThreads = numThreads; }
	void Start(const ThreadInitCallback& cb = ThreadInitCallback());

	EventLoop* GetNextLoop();
	EventLoop* GetLoopForHash(size_t hashCode);
	std::vector<EventLoop*> GetAllLoops();

	bool Started() const { return m_bStarted; }

	const string&  Name() const { return m_strName; }

private:
	EventLoop* m_baseLoop;
	string m_strName;
	bool m_bStarted;
	int32 m_numThreads;
	int32 m_next;
	std::vector<std::unique_ptr<EventLoopThread> > m_threads;
	std::vector<EventLoop*> m_loops;
};


#endif
