#include "thread_pool.h"
#include "count_down_latch.h"
#include "logger.h"

void print()
{
	DEBUG("tid=%d", CurrentThread::Tid());
}

void printString(const std::string& str)
{
	INFO("%s", str.c_str());
	usleep(100 * 1000);
}

void test(int32 maxSize)
{
	INFO("Test ThreadPool with max queue size = %d", maxSize);
	ThreadPool pool("MainThreadPool");
	pool.SetMaxQueueSize(maxSize);
	pool.Start(5);
	INFO("Adding");
	pool.Run(print);
	pool.Run(print);
	for (int i = 0; i < 1000; ++i)
	{
		char buf[32];
		snprintf(buf, sizeof buf, "task %d", i);
		pool.Run(std::bind(printString, std::string(buf)));
	}
	INFO("Done");

	CountDownLatch latch(1);
	pool.Run(std::bind(&CountDownLatch::CountDown, &latch));
	latch.Wait();
	pool.Stop();
}

int main()
{
	InitLogger("../log/test.log", "DEBUG");

	//test(0);
	//test(1);
	//test(5);
	//test(10);
	test(100);
	return 0;
}
