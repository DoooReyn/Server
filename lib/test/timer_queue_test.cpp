#include "event_loop.h"
#include "event_loop_thread.h"
#include "thread.h"
#include "logger.h"


int cnt = 0;
EventLoop* g_loop;

void printTid()
{
	printf("pid = %d, tid = %d\n", getpid(), CurrentThread::Tid());
	printf("now %s\n", Timestamp::Now().ToString().c_str());
}

void print(const char* msg)
{
	printf("msg %s %s\n", Timestamp::Now().ToString().c_str(), msg);
	if (++cnt == 20)
	{
		g_loop->Quit();
	}
}

void cancel(TimerId timer)
{
	g_loop->Cancel(timer);
	printf("cancelled at %s\n", Timestamp::Now().ToString().c_str());
}

int main()
{
	InitLogger("../log/test.log", "error");
	printTid();
	sleep(1);
	{
		EventLoop loop;
		g_loop = &loop;

		print("main");
		loop.RunAfter(1, std::bind(print, "once1"));
		loop.RunAfter(1.5, std::bind(print, "once1.5"));
		loop.RunAfter(2.5, std::bind(print, "once2.5"));
		loop.RunAfter(3.5, std::bind(print, "once3.5"));
		TimerId t45 = loop.RunAfter(4.5, std::bind(print, "once4.5"));
		loop.RunAfter(4.2, std::bind(cancel, t45));
		loop.RunAfter(4.8, std::bind(cancel, t45));
		loop.RunEvery(2, std::bind(print, "every2"));
		TimerId t3 = loop.RunEvery(3, std::bind(print, "every3"));
		loop.RunAfter(9.001, std::bind(cancel, t3));

		loop.Loop();
		print("main loop exits");
	}

	sleep(1);
	{
		EventLoopThread loopThread;
		EventLoop* loop = loopThread.StartLoop();
		loop->RunAfter(2, printTid);
		sleep(3);
		print("thread loop exits");
	}


	return 0;
}
