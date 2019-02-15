#include "memory_pool.h"




List* listCreate(void)
{
	List* list;
	if ((list = (List*)malloc(sizeof(*list))) == NULL)
	{
		return NULL;
	}

	list->head = NULL;
	list->len = 0;
	return list;
}

void listRelease(List* list)
{
	unsigned long len = list->len;
	ListNode* current = list->head;
	ListNode* next;
	while (len--)
	{
		next = current->next;
		free(current);
		current = next;
	}
	free(list);
}



List* listAddNode(List* list, ListNode* node)
{
	if (list->len == 0)
	{
		list->head = node;
		node->next = NULL;
	}
	else
	{
		node->next = list->head;
		list->head = node;
	}
	list->len++;
	return list;
}

List* listAddNewNode(List* list, void* value)
{

	ListNode* node;

	if ((node = (ListNode*)malloc(sizeof(*node))) == NULL)
	{
		return NULL;
	}
	node->value = value;
	return listAddNode(list, node);
}

ListNode* listGetNode(List* list)
{
	if (list->len == 0)
	{
		return NULL;
	}
	ListNode* node = list->head;
	list->head = node->next;
	list->len--;
	return node;
}

void listPrint(List* list)
{
	int n = list->len;
	printf("n:%d", n);
}

int RoundUP(int size)
{
	return (((size) + (size_t)__ALIGN - 1) & ~((size_t)__ALIGN - 1));
}

CMemPool::CMemPool()
{
	Init();
}

CMemPool::~CMemPool()
{
	for (int i = 0; i < POOLSIZE; i++)
	{
		listRelease(listPool[i]);
	}
}

void CMemPool::Init()
{
	for (int i = 0; i < POOLSIZE; i++)
	{
		listPool[i] = listCreate();
	}
}

void* CMemPool::GetBuff(int size)
{
	int realsize = RoundUP(size);
	int index = realsize / __ALIGN - 1;
	if (index >= POOLSIZE || index < 0)
	{
		void* pdata = malloc(realsize);
		return pdata;
	}

	List* pool = listPool[index];
	MutexLockGuard lock(m_muext);
	ListNode* pNode = listGetNode(pool);
	if (pNode == NULL)
	{
		for (int i = 0; i < ADDSIZE; i++)
		{
			void* pdata = malloc(realsize + PTRSIZE);
			if (pdata == NULL)
			{
				return NULL;
			}
			listAddNewNode(pool, pdata);
		}
		pNode = listGetNode(pool);
		if (pNode == NULL)
		{
			return NULL;
		}
	}
	uint64* ptrvalue = (uint64*)(pNode->value);
	ptrvalue[0] = (uint64)pNode;
	return &ptrvalue[1];
}

void CMemPool::DelBuff(void* value, int size)
{
	int realsize = RoundUP(size);
	int index = realsize / __ALIGN - 1;
	if (index >= POOLSIZE || index < 0)
	{
		free(value);
		value = NULL;
		return;
	}
	MutexLockGuard lock(m_muext);
	List* pool = listPool[index];
	uint64* ptrvalue = (uint64*)((char*)value - PTRSIZE);
	ListNode* pNode = (ListNode*)ptrvalue[0];
	listAddNode(pool, pNode);
}
