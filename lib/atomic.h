#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include "type_define.h"
#include "nocopyable.h"

template <typename T>
class AtomicIntegerT : private Nocopyable
{
public:
	AtomicIntegerT() : m_value(0)
	{
	}
	T Get()
	{
		return __sync_val_compare_and_swap(&m_value, 0, 0);
	}

	T GetAndAdd(T x)
	{
		return __sync_fetch_and_add(&m_value, x);
	}

	T AddAndGet(T x)
	{
		return GetAndAdd(x) + x;
	}

	T IncrementAndGet()
	{
		return AddAndGet(1);
	}

	T DecrementAndGet()
	{
		return AddAndGet(-1);
	}

	void Add(T x)
	{
		GetAndAdd(x);
	}

	void Increment()
	{
		IncrementAndGet();
	}

	void Decrement()
	{
		DecrementAndGet();
	}

	T GetAndSet(T newValue)
	{
		return __sync_lock_test_and_set(&m_value, newValue);
	}
private:
	volatile T m_value;
};
typedef AtomicIntegerT<int32> AtomicInt32;
typedef AtomicIntegerT<int64> AtomicInt64;

#endif
