#include "event_loop_thread_pool.h"
#include "event_loop.h"
#include "thread.h"
#include "logger.h"

void print(EventLoop* p = NULL)
{
	printf("main(): pid = %d, tid = %d, loop = %p\n", getpid(), CurrentThread::Tid(), p);
}

void init(EventLoop* p)
{
	printf("init(): pid = %d, tid = %d, loop = %p\n", getpid(), CurrentThread::Tid(), p);
}

int main()
{
	InitLogger("../log/test.log", "info");
	print();
	EventLoop loop;
	loop.RunAfter(11, std::bind(&EventLoop::Quit, &loop));

	{
		printf("Single thread %p:\n", &loop);
		EventLoopThreadPool model(&loop, "single");
		model.SetThreadNum(0);
		model.Start(init);
		assert(model.GetNextLoop() == &loop);
		assert(model.GetNextLoop() == &loop);
		assert(model.GetNextLoop() == &loop);
	}

	{
		printf("Another thread:\n");
		EventLoopThreadPool model(&loop, "another");
		model.SetThreadNum(1);
		model.Start(init);
		EventLoop* nextLoop = model.GetNextLoop();
		nextLoop->RunAfter(2, std::bind(print, nextLoop));
		assert(nextLoop != &loop);
		assert(nextLoop == model.GetNextLoop());
		assert(nextLoop == model.GetNextLoop());
		::sleep(3);

	}

	{
		printf("Three threads:\n");
		EventLoopThreadPool model(&loop, "three");
		model.SetThreadNum(3);
		model.Start(init);
		EventLoop* nextLoop = model.GetNextLoop();
		nextLoop->RunInLoop(std::bind(print, nextLoop));
		assert(nextLoop != &loop);
		assert(nextLoop != model.GetNextLoop());
		assert(nextLoop != model.GetNextLoop());
		assert(nextLoop == model.GetNextLoop());
	}

	return 0;
}
