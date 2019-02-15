#ifndef __CONDITION_H__
#define __CONDITION_H__
#include "nocopyable.h"
#include "type_define.h"
#include "mutex.h"

class Condition : private Nocopyable
{
public:
	explicit Condition(Mutex& mutex) : m_mutex(mutex)
	{
		pthread_cond_init(&m_cond, nullptr);
	}
	~Condition()
	{
		pthread_cond_destroy(&m_cond);
	}

	void Wait()
	{
		Mutex::UnassignGuard ug(m_mutex);
		pthread_cond_wait(&m_cond, m_mutex.GetPthreadMutex());
	}

	void Notify()
	{
		pthread_cond_signal(&m_cond);
	}

	void NotifyAll()
	{
		pthread_cond_broadcast(&m_cond);
	}

private:
	Mutex& m_mutex;
	pthread_cond_t m_cond;
};
#endif
