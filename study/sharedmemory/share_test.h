#ifndef __SHARE_TEST_H__
#define  __SHARE_TEST_H__
#include "share_memory.h"


struct SM_TestObj : public ShareObject
{
	int32 id = 0;
	int32 skill = 0;
	int32 high = 0;
	int32 length = 0;
	char name[15] = { 0 };
};


typedef SharedMemory<SM_TestObj> TestDataPool;


#endif