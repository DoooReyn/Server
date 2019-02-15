#include "type_define.h"
#include "adlist.h"


list* listCreate(void)
{
	struct list* list;
	if ((list = (struct list*)malloc(sizeof(*list))) == nullptr)
	{
		return nullptr;
	}

	list->head = list->tail = nullptr;
	list->len = 0;
	list->dup = nullptr;
	list->free = nullptr;
	list->match = nullptr;
	return list;
}


void listRelease(list* list)
{
	unsigned long len = list->len;
	listNode* current = list->head;
	listNode* next;
	while (len--)
	{
		next = current->next;
		if (list->free)
		{
			list->free(current->value);
		}
		free(current);
		current = next;
	}
	free(list);
}

list* listAddNodeHead(list* list, void* value)
{
	listNode* node;

	if ((node = (listNode*)malloc(sizeof(*node))) == nullptr)
	{
		return nullptr;
	}
	node->value = value;
	if (list->len == 0)
	{
		list->head = list->tail = node;
		node->prev = node->next = nullptr;
	}
	else
	{
		node->prev = nullptr;
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}
	list->len++;
	return list;
}

list* listAddNodeTail(list* list, void* value)
{

	listNode* node;

	if ((node = (listNode*)malloc(sizeof(*node))) == nullptr)
	{
		return nullptr;
	}
	node->value = value;
	if (list->len == 0)
	{
		list->head = list->tail = node;
		list->head->prev = NULL;
		list->tail->next = NULL;
	}
	else
	{
		node->prev = list->tail;
		node->next = nullptr;
		list->tail->next = node;
		list->tail = node;
	}
	list->len++;
	return list;
}


list* listInsertNode(list* list, listNode* old_node, void* value, int after)
{
	listNode* node;

	if ((node = (listNode*)malloc(sizeof(*node))) == nullptr)
	{
		return nullptr;
	}

	node->value = value;
	if (after)
	{
		node->prev = old_node;
		node->next = old_node->next;
		if (list->tail == old_node)
		{
			list->tail = node;
		}
	}
	else
	{
		node->next = old_node;
		node->prev = old_node->prev;
		if (list->head == old_node)
		{
			list->head = node;
		}
	}
	if (node->prev != nullptr)
	{
		node->prev->next = node;
	}
	if (node->next != nullptr)
	{
		node->next->prev = node;
	}
	list->len++;
	return list;
}

void listDelNode(list* list, listNode* node)
{
	if (node->prev)
	{
		node->prev->next = node->next;
	}
	else
	{
		list->head = node->next;
	}

	if (node->next)
	{
		node->next->prev = node->prev;
	}
	else
	{
		list->tail = node->prev;
	}
	if (list->free)
	{
		list->free(node->value);
	}

	free(node);
	list->len--;
}

listIter* listGetIterator(list* list, int direction)
{
	listIter* iter;

	if ((iter = (listIter*)malloc(sizeof(*iter))) == nullptr)
	{
		return nullptr;
	}

	if (direction == AL_START_HEAD)
	{
		iter->next = list->head;
	}
	else
	{
		iter->next = list->tail;
	}
	iter->direction = direction;
	return iter;
}

void listReleaseIterator(listIter* iter)
{
	free(iter);
}

void listRewind(list* list, listIter* li)
{
	li->next = list->head;
	li->direction = AL_START_HEAD;
}

void listRewindTail(list* list, listIter* li)
{
	li->next = list->tail;
	li->direction = AL_START_TAIL;
}

listNode* listNext(listIter* iter)
{
	listNode* current = iter->next;

	if (current != nullptr)
	{
		if (iter->direction == AL_START_HEAD)
		{
			iter->next = current->next;
		}
		else
		{
			iter->next = current->prev;
		}
	}
	return current;
}

list* listDup(list* orig)
{
	list* copy;
	listIter* iter;
	listNode* node;

	if ((copy = listCreate()) == nullptr)
	{
		return nullptr;
	}
	copy->dup = orig->dup;
	copy->free = orig->free;
	copy->match = orig->match;
	iter = listGetIterator(orig, AL_START_HEAD);
	while ((node = listNext(iter)) != nullptr)
	{
		void* value;

		if (copy->dup)
		{
			value = copy->dup(node->value);
			if (value == nullptr)
			{
				listRelease(copy);
				listReleaseIterator(iter);
				return nullptr;
			}
		}
		else
		{
			value = node->value;
		}
		if (listAddNodeTail(copy, value) == nullptr)
		{
			listRelease(copy);
			listReleaseIterator(iter);
			return nullptr;
		}
	}
	listReleaseIterator(iter);
	return copy;
}

listNode* listSearchKey(list* list, void* key)
{
	listIter* iter;
	listNode* node;

	iter = listGetIterator(list, AL_START_HEAD);
	while ((node = listNext(iter)) != nullptr)
	{
		if (list->match)
		{
			if (list->match(node->value, key))
			{
				listReleaseIterator(iter);
				return node;
			}
		}
		else
		{
			if (key == node->value)
			{
				listReleaseIterator(iter);
				return node;
			}
		}
	}
	listReleaseIterator(iter);
	return nullptr;
}

listNode* listIndex(list* list, long index)
{
	listNode* n;

	if (index < 0)
	{
		index = (-index) - 1;
		n = list->tail;
		while (index-- && n)
		{
			n = n->prev;
		}
	}
	else
	{
		n = list->head;
		while (index-- && n)
		{
			n = n->next;
		}
	}
	return n;
}

void listRotate(list* list)
{
	listNode* tail = list->tail;

	if (LIST_LENGTH(list) <= 1)
	{
		return;
	}

	/* Detach current tail */
	list->tail = tail->prev;
	list->tail->next = nullptr;
	/* Move it as head */
	list->head->prev = tail;
	tail->prev = nullptr;
	tail->next = list->head;
	list->head = tail;
}