#include "tcp_client.h"
#include "logger.h"
#include "thread.h"
#include "inet_address.h"
#include "event_loop.h"

int32 numThreads = 0;

class EchoClient;
std::vector<EchoClient*> vecClient;

int32 current = 0;

class EchoClient : private Nocopyable
{
public:
	EchoClient(EventLoop* loop, const InetAddress& listenAddr, const string& id)
		: m_pLoop(loop)
		, m_client(loop, listenAddr, "EchoClient" + id)
	{
		m_client.SetConnectionCallback(std::bind(&EchoClient::onConnection, this, _1));
		m_client.setMessageCallback(std::bind(&EchoClient::onMessage, this, _1, _2, _3));
	}

	void Connect()
	{
		m_client.Connect();
	}

private:

	void onConnection(const TcpConnectionPtr& conn)
	{
		DEBUG("%s -> %s is %d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
		if (conn->Connected())
		{
			++current;
			if (static_cast<size_t>(current) < vecClient.size())
			{
				vecClient[current]->Connect();
			}
			INFO("*** connected :%d", current);
		}
		conn->Send("world\n");
	}


	void onMessage(const TcpConnectionPtr& conn, BufferEx* buf, Timestamp time)
	{
		string msg(buf->RetrieveAllAsString());
		DEBUG("%s recv :%ld bytes at %s content:%s", conn->GetName().c_str(), msg.size(), time.ToString().c_str(), msg.c_str());
		if (msg == "quit\n")
		{
			conn->Send("bye\n");
			conn->Shutdown();
		}
		else if (msg == "shutdown\n")
		{
			m_pLoop->Quit();
		}
		else
		{
			conn->Send(msg);
		}
	}

	EventLoop* m_pLoop;
	TCPClient m_client;

};

int main(int argc, char* argv[])
{
	InitLogger("../log/test.log", "debug");
	INFO("pid = %d ,tid = %d", getpid(), CurrentThread::Tid());
	if (argc > 1)
	{
		EventLoop loop;
		bool ipv6 = argc > 3;
		InetAddress serverAddr(argv[1], 2001, ipv6);
		int32 n = 1;
		if (argc > 2)
		{
			n = atoi(argv[2]);
		}
		vecClient.reserve(n);
		for (int32 i = 0; i < n; ++i)
		{
			char buf[32] = { 0 };
			snprintf(buf, sizeof(buf), "%d", i + 1);
			vecClient.push_back(new EchoClient(&loop, serverAddr, buf));
		}
		vecClient[current]->Connect();
		loop.Loop();
	}
	else
	{
		printf("Usage: %s host_ip [current#]\n", argv[0]);
	}

	return 0;
}