#ifndef __MEMORY_POOL__
#define __MEMORY_POOL__

#include "type_define.h"
#include "singleton.h"
#include "mutex.h"

#define		SIZENUM		10
#define		__ALIGN		64
#define		POOLSIZE	100
#define		ADDSIZE		128
#define		PTRSIZE		8


typedef struct ListNode
{
	void* value;
	struct ListNode* next;
} ListNode;

typedef struct List
{
	ListNode* head;
	unsigned long len;
} List;

List* listCreate(void);
void listRelease(List* list);
List* listAddNode(List* list, ListNode* node);
List* listAddNewNode(List* list, void* value);
ListNode* listGetNode(List* list);
void listPrint(List* list);
int32 RoundUP(int32 size);


class CMemPool : public SingletonBase<CMemPool>
{
public:
	CMemPool();
	~CMemPool();
	void* GetBuff(int size);
	void DelBuff(void* value, int size);
private:
	void Init();
private:
	Mutex m_muext;
	List* listPool[POOLSIZE];
};


#define MEM_OBJECT_POOL_SUPORT_MT(ClassName) \
	typedef MemObjectPool<##ClassName##> PoolType; \
	static void* operator new(size_t size) \
	{ \
		if(size != sizeof(ClassName))\
		{\
			assert(false);\
		}\
		ClassName* ptr = GetObjectPool()->CreateMemObject(); \
		return ptr;\
	} \
	static void operator delete(void* pointee) \
	{\
		ClassName* ptr = (ClassName*)pointee;\
		GetObjectPool()->FreeMemObject(ptr);\
	} \
	static PoolType* GetObjectPool() \
	{\
	static PoolType s_MemObjectPool;\
	return &s_MemObjectPool;\
}


template <typename T>
class MemObjectPool
{
public:
	MemObjectPool(void)
	{
		m_list = listCreate();
	}
	~MemObjectPool(void)
	{
		listRelease(m_list);
	}
public:
	T*		CreateMemObject();
	void    FreeMemObject(T* pObj);
private:
	bool	AllocpNodeBuffer(int nSize = 64);
private:
	List* m_list;
};


template <typename T>
T* MemObjectPool<T>::CreateMemObject()
{
	if (m_list->len == 0)
	{
		AllocpNodeBuffer();
	}

	if (m_list->len == 0)
	{
		return NULL;
	}

	ListNode* pNode = listGetNode(m_list);
	uint64* pdata = (uint64*)(pNode->value);
	pdata[0] = (uint64)pNode;
	return (T*)&pdata[1];
}



template <typename T>
void MemObjectPool<T>::FreeMemObject(T* pObj)
{
	if (pObj == NULL)
	{
		return;
	}

	uint64* pdata = (uint64*)((char*)pObj - PTRSIZE);
	ListNode* pNode = (ListNode*)pdata[0];
	listAddNode(m_list, pNode);
	return;
}


template <typename T>
bool MemObjectPool<T>::AllocpNodeBuffer(int32 nSize /*= 64*/)
{
	int32 realsize = sizeof(T) + PTRSIZE;
	char* pdata = (char*)malloc(realsize * nSize);
	if (pdata == NULL)
	{
		return false;
	}

	memset(pdata, 0, realsize * nSize);
	for (int32 i = 0; i < nSize; i++)
	{
		listAddNewNode(m_list, pdata);
		pdata += realsize;
	}
	return true;
}


#endif