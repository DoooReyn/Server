#include "tcp_client.h"
#include "logger.h"
#include "event_loop.h"
#include "socket_ops.h"
#include "connector.h"
#include "common.h"


void RemoveConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
	loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
}

void RemoveConnector(const ConnectorPtr& connnector)
{
	DEBUG("RemoveConnector");
}


TCPClient::TCPClient(EventLoop* loop, const InetAddress& serverAddr, const string& name)
	: m_pLoop(loop)
	, m_connector(new Connector(loop, serverAddr))
	, m_strName(name)
	, m_connectionCallback(DefaultConnectionCallback)
	, m_disconnectCallback(DefaultDisconnectionCallback)
	, m_messageCallback(DefaultMessageCallback)
	, m_bRetry(false)
	, m_bConnect(true)
	, m_nextConnId(1)
{
	m_connector->SetNewConnectionCallback(std::bind(&TCPClient::NewConnection, this, _1));
	//DEBUG("TCPClient::TcpClient[%s] - connector %p", m_strName.c_str(), m_connector.get());
}

TCPClient::~TCPClient()
{
	//DEBUG("TCPClient::~TcpClient[%s] - connector %p", m_strName.c_str(), m_connector.get());

	TcpConnectionPtr conn;
	bool unique = false;
	{
		MutexLockGuard lock(m_mutex);
		unique = m_connection.unique();
		conn = m_connection;
	}

	if (conn)
	{
		assert(m_pLoop == conn->GetLoop());
		CloseCallback cb = std::bind(&::RemoveConnection, m_pLoop, _1);
		m_pLoop->RunInLoop(std::bind(&TcpConnection::SetCloseCallback, conn, cb));
		if (unique)
		{
			conn->ForceClose();
			//INFO("~TCPClient conn:%p unique:%d", conn, unique);
		}
	}
	else
	{
		m_connector->Stop();
		m_pLoop->RunAfter(1, std::bind(&::RemoveConnector, m_connector));
	}
}

void TCPClient::Connect()
{
	//DEBUG("TCPClient::Connect[%s] - connecting to %s", m_strName.c_str(), m_connector->ServerAddr().ToIPPort().c_str());
	m_bConnect = true;
	m_connector->Start();
}

void TCPClient::Disconnect()
{
	m_bConnect = false;
	{
		MutexLockGuard lock(m_mutex);
		if (m_connection)
		{
			m_connection->Shutdown();
		}
	}
}

void TCPClient::Stop()
{
	m_bConnect = false;
	m_connector->Stop();
}

void TCPClient::NewConnection(int32 sockfd)
{
	m_pLoop->AssertInLoopThread();
	InetAddress peerAddr(GetPeerAddr(sockfd));
	char buf[32] = { 0 };
	snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.ToIPPort().c_str(), m_nextConnId);
	++m_nextConnId;
	string connName = m_strName + buf;
	InetAddress localAddr(::GetLocalAddr(sockfd));


	TcpConnectionPtr conn(new TcpConnection(m_pLoop, connName, sockfd, localAddr, peerAddr));

	conn->SetConnectionCallback(m_connectionCallback);
	conn->SetDisconnectCallback(m_disconnectCallback);
	conn->SetMessageCallback(m_messageCallback);
	conn->SetWriteCompleteCallback(m_writeCompleteCallback);
	conn->SetCloseCallback(std::bind(&TCPClient::removeConection, this, _1));
	{
		MutexLockGuard lock(m_mutex);
		m_connection = conn;
	}

	conn->ConnectEstablished();
	m_timerid = m_pLoop->RunEvery(HEART_BEAT_TIME, std::bind(&TCPClient::onHeartBeatSend, this));
}

void TCPClient::removeConection(const TcpConnectionPtr& conn)
{
	m_pLoop->AssertInLoopThread();
	assert(m_pLoop == conn->GetLoop());

	{
		MutexLockGuard lock(m_mutex);
		assert(conn == m_connection);
		m_connection.reset();
	}

	m_pLoop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));

	if (m_bConnect && m_bRetry)
	{
		INFO("TCPClient::Connect[%s] - Reconnecting to %s", m_strName.c_str(), m_connector->ServerAddr().ToIPPort().c_str());
		m_connector->Restart();
	}
	m_pLoop->Cancel(m_timerid);
}

void TCPClient::onHeartBeatSend()
{
	CmdConnectHeartBeat sendPack;
	sendPack.requesttime = Timestamp::Now().GetSeconds();
	m_connection->Send(CMD_CONNECT_HEART_BEAT, &sendPack, sizeof(sendPack));
	m_connection->SetHeartBeat();
}

