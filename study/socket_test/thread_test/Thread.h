#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	void Start();
	void Join();

private:
	virtual void Run() = 0;
	static void* ThreadRoutine(void* arg);
private:
	pthread_t m_threadId;
};

#endif
