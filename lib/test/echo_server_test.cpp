#include "tcp_server.h"
#include "logger.h"
#include "thread.h"
#include "inet_address.h"
#include "event_loop.h"
#include "signal_catch.h"

int32 numThreads = 0;
class EchoServer
{
public:
	EchoServer(EventLoop* loop, const InetAddress& listenAddr)
		: m_loop(loop)
		, m_server(m_loop, listenAddr, "EchoServer")
	{
		m_server.SetConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
		m_server.SetMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
		m_server.SetThreadNum(numThreads);
	}

	void Start()
	{
		INFO("EchoServer Start ThreadNum %d", numThreads);
		m_server.Start();
	}
private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		DEBUG("%s -> %s is %d", conn->GetPeerAddr().ToIPPort().c_str(), conn->GetLocalAddr().ToIPPort().c_str(), conn->Connected());
		INFO("%s", conn->GetTcpInfoString().c_str());
		//conn->Send("hello\n");
	}

	void onMessage(const TcpConnectionPtr& conn, BufferEx* buf, Timestamp time)
	{
		string msg(buf->RetrieveAllAsString());
		DEBUG("%s recv %lu bytes at %s content:%s", conn->GetName().c_str(), msg.size(), time.ToString().c_str(), msg.c_str());
		if (msg == "exit\n")
		{
			//conn->Send("bye\n");
			conn->Shutdown();
		}
		if (msg == "quit\n")
		{
			m_loop->Quit();
		}
		//conn->Send(msg);
	}
	EventLoop* 	m_loop;
	TcpServer m_server;
};

int main(int argc, char* argv[])
{
	SetSignedCatched();
	InitLogger("../log/test.log", "debug");
	INFO("pid = %d, tid =%d", getpid(), CurrentThread::Tid());
	INFO("sizeof TcpConnection %lu", sizeof(TcpConnection));
	if (argc > 1)
	{
		numThreads = atoi(argv[1]);
	}
	bool ipv6 = argc > 2;
	EventLoop loop;
	InetAddress listenAddr(2001, false, ipv6);
	EchoServer server(&loop, listenAddr);
	server.Start();
	loop.Loop();
	return 0;
}
