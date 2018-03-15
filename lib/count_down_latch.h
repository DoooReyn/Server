#ifndef __COUNTDOWNLATCH_H__
#define __COUNTDOWNLATCH_H__


#include "nocopyable.h"
#include "mutex.h"
#include "condition.h"

class CountDownLatch : private Nocopyable
{
public:
	explicit CountDownLatch(int32 count);
	void Wait();
	void CountDown();
	int32 GetCount() const;
private:
	mutable Mutex m_mutex;
	Condition m_condition;
	int32 m_count;
};


#endif
