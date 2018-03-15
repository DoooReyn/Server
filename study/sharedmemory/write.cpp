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

int main()
{

	TestDataPool sharedObj("SHARE_TEST", 1024, true);
	Print(sharedObj);

	for (int32 i = 0 ; i < 4097 ; i++)
	{
		SM_TestObj* pData = sharedObj.NewObject(true);
		if (pData == NULL)
		{
			cout << "error newobject" << endl;
			return -1;
		}
		pData->id = i;
		pData->high = i * 2;
		pData->length = i * 4;

		char szName[15] = { 0 };
		sprintf(szName, "Test_%d", i);
		strncpy(pData->name, szName, sizeof(pData->name));
		pData->Modify();
	}
	Print(sharedObj);

	sleep(600 * 1000);
	return 0;
}
