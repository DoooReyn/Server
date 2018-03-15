#include "share_test.h"
#include "share_memory.h"
#include <unistd.h>
using namespace std;


void Print(TestDataPool& pool)
{
	cout << "page :" << pool.GetPage();
	cout << " count :" << pool.GetCount();
	cout << " used :" << pool.GetUsedCount();
	cout << " free :" << pool.GetFreeCount();
	cout << endl;
}

void Print(SM_TestObj& obj)
{
	cout << "id :" << obj.id;
	cout << " skill:" << obj.skill;
	cout << " high :" << obj.high / 2;
	cout << " length :" << obj.length / 4;
	cout << " name :" << obj.name;
	cout << endl;
}


int main()
{
	TestDataPool sharedObj("SHARE_TEST", 1024, false);
	Print(sharedObj);

	int32 count = sharedObj.GetRawObjectCount();

	for (int32 i = 0; i < count; i++)
	{
		_SMBlock* pBlock = sharedObj.GetSMBbyRawIndex(i);
		if (pBlock == NULL)
		{
			printf("GetSMBbyRawIndex error i:%d \n", i);
			return -1;
		}

		SM_TestObj* pdata = sharedObj.GetObjectByRawindex(i);
		if (pdata == NULL)
		{
			printf("getObjectByRawindex error i:%d \n", i);
			return -1;
		}
		if (pdata->m_nFlag != 0xFE)
		{
			continue;
		}
		if (pdata->IsModify())
		{
			Print(*pdata);
		}

	}


	sleep(600 * 1000);

	return 0;
}
