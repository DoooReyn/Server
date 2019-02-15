#include "event_loop.h"
#include "thread.h"
#include "logger.h"

EventLoop* g_loop;

void callback()
{
	printf("callback(): pid = %d, tid = %d\n", getpid(), CurrentThread::Tid());
	EventLoop anotherLoop;
}

void threadFunc()
{
	printf("threadFunc(): pid = %d, tid = %d\n", getpid(), CurrentThread::Tid());

	assert(EventLoop::GetEventLoopOfCurrentThread() == nullptr);
	EventLoop loop;
	assert(EventLoop::GetEventLoopOfCurrentThread() == &loop);
	loop.RunAfter(1.0, callback);
	loop.Loop();
}

int main()
{
	InitLogger("../log/test.log", "error");
	printf("main(): pid = %d, tid = %d\n", getpid(), CurrentThread::Tid());

	assert(EventLoop::GetEventLoopOfCurrentThread() == nullptr);
	EventLoop loop;
	assert(EventLoop::GetEventLoopOfCurrentThread() == &loop);
	Thread thread(threadFunc);
	thread.Start();

	loop.Loop();
}