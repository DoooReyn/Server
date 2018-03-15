#include "count_down_latch.h"


CountDownLatch::CountDownLatch(int32 count)
	: m_mutex()
	, m_condition(m_mutex)
	, m_count(count)
{
}
void CountDownLatch::Wait()
{
	MutexLockGuard lock(m_mutex);
	while(m_count > 0)
	{
		m_condition.Wait();
	}
}

void CountDownLatch::CountDown()
{
	MutexLockGuard lock(m_mutex);
	--m_count;
	if(m_count == 0)
	{
		m_condition.NotifyAll();
	}
}

int32 CountDownLatch::GetCount() const
{
	MutexLockGuard lock(m_mutex);
	return m_count;
}
