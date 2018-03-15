#include "string_tool.h"
#include "blocking_queue.h"

void TestBlockQueue()
{
	BlockingQueue<std::string> queue;
	for(int32 i = 0; i < 1000 ; i++)
	{
		string strTmp = StringTool::Format("%d", i);
		queue.put(strTmp);
	}

	printf("queue size:%lu\n", queue.size());
	for (int32 i = 0 ; i < 10; i++)
	{
		string strTmp = queue.take();
		printf("queue: %s\n", strTmp.c_str());
	}
	printf("queue size:%lu\n", queue.size());
}

int main(int argc, char const* argv[])
{
	TestBlockQueue();
	return 0;
}