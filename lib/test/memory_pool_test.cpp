#include "memory_pool.h"
#include "logger.h"
#include "thread.h"

void TestMemPool()
{
	for (int32 i = 1; i <= 1040; i++)
	{
		char* cmd_buf = (char*)CMemoryPools::Instance().GetBuff(i);
		CMemoryPools::Instance().DelBuff(cmd_buf);
	}

}

int main(int argc, char const* argv[])
{
	InitLogger("./log/test.log", "debug");

	int kThreads = 1000;
	for (int i = 0; i < kThreads; ++i)
	{
		Thread t7(TestMemPool);
		t7.Start();
		t7.Join();
	}
	CMemoryPools::Instance().DisplayMemoryList();
	return 0;
}




