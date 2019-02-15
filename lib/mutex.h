#ifndef __MUTXT_H__
#define __MUTXT_H__
#include "nocopyable.h"
#include "type_define.h"
#include "thread.h"

class Mutex : private Nocopyable
{
public:
	Mutex()
	{
		pthread_mutex_init(&m_mutex, nullptr);
	}

	~Mutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	void Lock()
	{
		pthread_mutex_lock(&m_mutex);
		AssignHolder();
	}

	void UnLock()
	{
		UnassignHolder();
		pthread_mutex_unlock(&m_mutex);
	}

	bool TryLock()
	{
		return (pthread_mutex_trylock(&m_mutex) == 0);
	}

	pthread_mutex_t* GetPthreadMutex()
	{
		return &m_mutex;
	}

	void AssignHolder()
	{
		m_holder = CurrentThread::Tid();
	}

	void UnassignHolder()
	{
		m_holder = 0;
	}

	bool isLockedByThisThread() const
	{
		return m_holder == CurrentThread::Tid();
	}

	void AssertLocked() const
	{
		assert(isLockedByThisThread());
	}

private:
	friend class Condition;
	class UnassignGuard : private Nocopyable
	{
	public:
		explicit UnassignGuard(Mutex& owner) :  mutex_(owner)
		{
			mutex_.UnassignHolder();
		}
		~UnassignGuard()
		{
			mutex_.AssignHolder();
		}
	private:
		Mutex& mutex_;
	};
	pthread_mutex_t m_mutex;
	pid_t m_holder;
};


class MutexLockGuard : private Nocopyable
{
public:
	explicit MutexLockGuard(Mutex& mutex) : m_mutex(mutex)
	{
		m_mutex.Lock();
	}

	~MutexLockGuard()
	{
		m_mutex.UnLock();
	}
private:
	Mutex& m_mutex;
};

class RWLock : private Nocopyable
{
public:
	RWLock()
	{
		pthread_rwlock_init(&m_rwlock, nullptr);
	}
	~RWLock()
	{
		pthread_rwlock_destroy(&m_rwlock);
	}
	void rdlock()
	{
		pthread_rwlock_rdlock(&m_rwlock);
	}
	void wrlock()
	{
		pthread_rwlock_wrlock(&m_rwlock);
	}

	void unlock()
	{
		pthread_rwlock_unlock(&m_rwlock);
	}

private:
	pthread_rwlock_t m_rwlock;
};
#endif
