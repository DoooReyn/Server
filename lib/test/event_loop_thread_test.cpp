#include "event_loop_thread.h"
#include "event_loop.h"
#include "thread.h"
#include "logger.h"
#include "count_down_latch.h"

EventLoop* g_loop;

void print(EventLoop* p = NULL)
{
	printf("print: pid = %d, tid = %d, loop = %p\n", getpid(), CurrentThread::Tid(), p);
}

void quit(EventLoop* p)
{
	print(p);
	p->Quit();
}


int main()
{
	InitLogger("../log/test.log", "error");
	print();

	{
		EventLoopThread thr1;  // never start
	}

	{
		// dtor calls quit()
		EventLoopThread thr2;
		EventLoop* loop = thr2.StartLoop();
		loop->RunInLoop(std::bind(print, loop));
		CurrentThread::SleepUsec(500 * 1000);
	}

	{
		// quit() before dtor
		EventLoopThread thr3;
		EventLoop* loop = thr3.StartLoop();
		loop->RunInLoop(std::bind(quit, loop));
		CurrentThread::SleepUsec(500 * 1000);
	}
}
