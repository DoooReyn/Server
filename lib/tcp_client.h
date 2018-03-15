#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "nocopyable.h"
#include "mutex.h"
#include "tcp_connection.h"

class Connector;

typedef std::shared_ptr<Connector> ConnectorPtr;

class TCPClient : private Nocopyable
{
public:
	TCPClient(EventLoop* loop, const InetAddress& serverAddr, const string& name);
	~TCPClient();

	void Connect();
	void Disconnect();
	void Stop();

	TcpConnectionPtr GetConnection() const
	{
		MutexLockGuard lock(m_mutex);
		return m_connection;
	}


	EventLoop* GetLoop() const
	{
		return m_pLoop;
	}
	bool Retry() const
	{
		return m_bRetry;
	}
	void EnableRetry()
	{
		m_bRetry = true;
	}

	const string& GetName() const
	{
		return m_strName;
	}


	void SetConnectionCallback(const ConnectionCallback& cb)
	{
		m_connectionCallback = cb;
	}

	void SetMessageCallback(const MessageCallback& cb)
	{
		m_messageCallback = cb;
	}

	void SetDisconnectCallback(const DisConnectCallback& cb)
	{
		m_disconnectCallback = cb;
	}

	void SetWriteCompleteCallback(const WriteCompleteCallback& cb)
	{
		m_writeCompleteCallback = cb;
	}

private:

	void NewConnection(int32 sockfd);
	void removeConection(const TcpConnectionPtr& conn);

	void onHeartBeatSend();
	EventLoop* m_pLoop;
	ConnectorPtr m_connector;
	const string m_strName;
	ConnectionCallback m_connectionCallback;
	DisConnectCallback m_disconnectCallback;
	MessageCallback m_messageCallback;
	WriteCompleteCallback m_writeCompleteCallback;
	bool m_bRetry;
	bool m_bConnect;
	int32 m_nextConnId;
	mutable Mutex m_mutex;
	TcpConnectionPtr m_connection;
	TimerId m_timerid;
};


#endif