#ifndef __ENTRY_H__
#define __ENTRY_H__
#include "type_define.h"

class Entry
{
public:
	Entry()
	{
		m_id = 0;
	}

	uint32 GetId()const
	{
		return m_id;
	}

	void SetId(uint32 id)
	{
		m_id = id;
	}
private:
	uint32 m_id;
};


//T 必须为Entry 类型，才能放入Entry管理器中，支持调用
//Callback 回调累，用于遍历EntryManager中的对象
template <typename T>
struct Callback
{
	virtual bool exec(T* e) = 0;
	virtual ~Callback() {};
};

extern __thread uint32 seedp;
inline int randBetween(int min, int max)
{
	if(min == max)
	{
		return min;
	}
	else if(min > max)
	{
		return max + (int)(((double)min - (double)max + 1.0) * rand_r(&seedp) / (RAND_MAX + 1.0));
	}
	else
	{
		return min + (int)(((double)max - (double)min + 1.0) * rand_r(&seedp) / (RAND_MAX + 1.0));
	}
}

#endif
