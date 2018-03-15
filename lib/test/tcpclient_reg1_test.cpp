#include "tcp_client.h"
#include "logger.h"
#include "event_loop.h"


TCPClient* g_client;

void timeout()
{
	INFO("timeout");
	g_client->Stop();
}




int main(int argc, char* argv[])
{
	InitLogger("/root/oschina/lib/log/test.log", "debug");

	EventLoop loop;
	InetAddress servAddr("127.0.0.1", 2);

	TCPClient client(&loop, servAddr, "TCPClient");
	g_client = &client;

	loop.RunAfter(0.0, timeout);
	loop.RunAfter(1.0, std::bind(&EventLoop::Quit, &loop));
	client.Connect();
	CurrentThread::SleepUsec(100 * 1000);
	loop.Loop();

	return 0;
}
