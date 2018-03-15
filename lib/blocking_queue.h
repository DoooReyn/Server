#ifndef __BLOCKING_QUEUE_H__
#define __BLOCKING_QUEUE_H__
#include "type_define.h"
#include "nocopyable.h"
#include "condition.h"
template<typename T>
class BlockingQueue : public Nocopyable
{
public:
	BlockingQueue() : m_mutex(), m_cond(m_mutex), m_deqQueue() {}

	void put(const T& x)
	{
		MutexLockGuard lock(m_mutex);
		m_deqQueue.push_back(x);
		m_cond.Notify();
	}

	T take()
	{
		MutexLockGuard lock(m_mutex);
		while(m_deqQueue.empty())
		{
			m_cond.Wait();
		}
		assert(m_deqQueue.empty() == false); //m_deqQueue can't be empty

		//get the front
		T front(m_deqQueue.front());
		m_deqQueue.pop_front();
		return front;
	}

	size_t size() const
	{
		MutexLockGuard lock(m_mutex);
		return m_deqQueue.size();
	}
private:
	mutable Mutex m_mutex;
	Condition m_cond;
	std::deque<T> m_deqQueue;
};
#endif
