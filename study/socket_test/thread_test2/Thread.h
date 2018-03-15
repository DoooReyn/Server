#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <boost/function.hpp>
class Thread
{
public:
	typedef boost::function<void ()> ThreadFunc;
	explicit Thread(const ThreadFunc& func);

	virtual ~Thread();

	void Start();
	void Join();
	void SetAutoDelete(bool autoDelete);
private:
	void Run();
	static void* ThreadRoutine(void* arg);
private:
	pthread_t m_threadId;
	ThreadFunc m_func;
	bool m_autoDelete;
};

#endif
