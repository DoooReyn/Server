//redis 源码中摘取
#ifndef __ADLIST_H__
#define __ADLIST_H__

typedef struct listNode
{
	struct listNode* prev;
	struct listNode* next;
	void* value;
} listNode;


typedef struct listIter
{
	listNode* next;
	int direction;
} listIter;

typedef struct list
{
	listNode* head;
	listNode* tail;
	void* (*dup)(void* ptr);
	void* (*free)(void* ptr);
	int(*match)(void* ptr, void* key);
	unsigned long len;
} list;

#define LIST_LENGTH(l) ((l)->len)
#define LIST_FIRST(l) ((l)->head)
#define LIST_LAST(l) ((l)->tail)
#define LIST_PRE_NODE(n) ((n)->prev)
#define LIST_NEXT_NODE(n) ((n)->next)
#define LIST_NODE_VALUE(n) ((n)->value)

#define LIST_SET_DUP_METHOD(l,m) ((l)->dup = (m))
#define LIST_SET_FREE_METHOD(l,m) ((l)->free = (m))
#define LIST_SET_MATCH_METHOD(l,m) ((l)->match = (m))

#define LIST_GET_DUP_METHOD(l) ((l)->dup)
#define LIST_GET_FREE(l) ((l)->free)
#define LIST_GET_MATCH(l) ((l)->match)

#define AL_START_HEAD 0
#define AL_START_TAIL 1


list* listCreate(void);
void listRelease(list* list);
list* listAddNodeHead(list* list, void* value);
list* listAddNodeTail(list* list, void* value);
list* listInsertNode(list* list, listNode* old_node, void* value, int after);
void listDelNode(list* list, listNode* node);
listIter* listGetIterator(list* list, int direction);
listNode* listNext(listIter* iter);
void listReleaseIterator(listIter* iter);
list* listDup(list* orig);
listNode* listSearchKey(list* list, void* key);
listNode* listIndex(list* list, long index);
void listRewind(list* list, listIter* li);
void listRewindTail(list* list, listIter* li);
void listRotate(list* list);

#endif