#include "tcp_client.h"
#include "logger.h"
#include "event_loop.h"
#include "thread.h"

void threadFunc(EventLoop* loop)
{
	InetAddress servaddr("127.0.0.1", 2001);
	TCPClient client(loop, servaddr, "TCPClinet");
	client.Connect();
	CurrentThread::SleepUsec(1000 * 1000);
}



int main(int argc, char* argv[])
{
	InitLogger("/root/oschina/lib/log/test.log", "debug");

	EventLoop loop;

	loop.RunAfter(3.0, std::bind(&EventLoop::Quit, &loop));

	Thread thr(std::bind(threadFunc, &loop));
	thr.Start();
	loop.Loop();

	return 0;
}
