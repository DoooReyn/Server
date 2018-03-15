#include "tcp_server.h"
#include "logger.h"
#include "acceptor.h"
#include "event_loop.h"
#include "event_loop_thread_pool.h"
#include "socket_ops.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& name, OPTION option /*= kNoReusePort*/)
	: m_pLoop(loop)
	, m_strIPPort(listenAddr.ToIPPort())
	, m_strName(name)
	, m_acceptor(new Acceptor(loop, listenAddr, option == kReusePort))
	, m_threadPool( new EventLoopThreadPool(loop, name))
	, m_connectionCallback(DefaultConnectionCallback)
	, m_disconnectCallback(DefaultDisconnectionCallback)
	, m_messageCallback(DefaultMessageCallback)
	, m_nextConnId(1)
{
	m_acceptor->SetNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
	m_pLoop->AssertInLoopThread();
	//DEBUG("TcpServer::~TcpServer [ %s ] destructing", m_strName.c_str());
	for (ConnectionMap::iterator it(m_mapConnection.begin()); it != m_mapConnection.end(); ++it)
	{
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		conn->GetLoop()->RunInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
		conn.reset();
	}
}

bool TcpServer::IsBindSuccess()
{
	if (m_acceptor.get())
	{
		return m_acceptor->Binded();
	}
	else
	{
		return false;
	}
}

void TcpServer::SetThreadNum(int32 numThreads)
{
	assert(0 <= numThreads);
	m_threadPool->SetThreadNum(numThreads);
}

void TcpServer::Start()
{
	if (m_started.GetAndSet(1) == 0)
	{
		m_threadPool->Start(m_threadInitCallback);
		assert(!m_acceptor->Listening());
		m_pLoop->RunInLoop(std::bind(&Acceptor::Listen, m_acceptor.get()));
	}
}

void TcpServer::newConnection(int32 sockfd, const InetAddress& peerAddr)
{
	m_pLoop->AssertInLoopThread();
	EventLoop* ioLoop = m_threadPool->GetNextLoop();
	char buf[64] = { 0 };
	snprintf(buf, sizeof(buf), "-%s#%d", m_strIPPort.c_str(), m_nextConnId);
	++m_nextConnId;
	string connName = m_strName + buf;

	//INFO("new connect [%s] form %s", connName.c_str(), peerAddr.ToIPPort().c_str());
	InetAddress localAddr(GetLocalAddr(sockfd));

	TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
	m_mapConnection[connName] = conn;
	conn->SetConnectionCallback(m_connectionCallback);
	conn->SetDisconnectCallback(m_disconnectCallback);

	conn->SetMessageCallback(m_messageCallback);
	conn->SetWriteCompleteCallback(m_writeCompleteCallback);
	conn->SetCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
	ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	m_pLoop->RunInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	m_pLoop->AssertInLoopThread();
	//INFO("TcpServer::removeConnectionInLoop [%s] - connection %s", m_strName.c_str(), conn->GetName().c_str());
	size_t n = m_mapConnection.erase(conn->GetName());
	assert(n == 1);
	EventLoop* ioLoop = conn->GetLoop();
	ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
}
