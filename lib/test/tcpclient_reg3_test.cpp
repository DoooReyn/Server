#include "tcp_client.h"
#include "logger.h"
#include "event_loop_thread.h"

int main(int argc, char* argv[])
{
	InitLogger("/root/oschina/lib/log/test.log", "debug");

	EventLoopThread loopThread;
	{
		InetAddress servAddr("127.0.0.1", 2001);
		TCPClient client(loopThread.StartLoop(), servAddr, "TCPClient1");
		client.Connect();
		CurrentThread::SleepUsec(500 * 1000);
		client.Disconnect();
	}

	CurrentThread::SleepUsec(1000 * 1000);
	return 0;
}
