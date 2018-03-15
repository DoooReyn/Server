#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "tcp_connection.h"
#include "atomic.h"


class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : private Nocopyable
{
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;
	enum OPTION
	{
		kNoReusePort,
		kReusePort,
	};

	TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& name, OPTION option = kNoReusePort);
	~TcpServer();

	bool IsBindSuccess();
	const string& IPPort() const { return m_strIPPort; }
	const string& Name() const { return m_strName; }
	EventLoop* GetLoop() const { return m_pLoop; }

	void SetThreadNum(int32 numThreads);
	void SetThreadInitCallback(const ThreadInitCallback& cb) { m_threadInitCallback = cb; }
	std::shared_ptr<EventLoopThreadPool> GetThreadPool() { return m_threadPool; }

	void Start();

	void SetConnectionCallback(const ConnectionCallback& cb) { m_connectionCallback = cb; }
	void SetDisconnectCallback(const DisConnectCallback& cb) { m_disconnectCallback = cb; }
	void SetMessageCallback(const MessageCallback& cb) { m_messageCallback = cb; }
	void SetWriteCompleteCallback(const WriteCompleteCallback& cb) { m_writeCompleteCallback = cb; }




private:

	void newConnection(int32 sockfd, const InetAddress& peerAddr);

	void removeConnection(const TcpConnectionPtr& conn);

	void removeConnectionInLoop(const TcpConnectionPtr& conn);


	typedef std::map<string, TcpConnectionPtr> ConnectionMap;

	EventLoop* m_pLoop;
	const string m_strIPPort;
	const string m_strName;
	std::shared_ptr<Acceptor> m_acceptor;
	std::shared_ptr<EventLoopThreadPool> m_threadPool;
	ConnectionCallback m_connectionCallback;
	DisConnectCallback m_disconnectCallback;

	MessageCallback	m_messageCallback;
	WriteCompleteCallback m_writeCompleteCallback;
	ThreadInitCallback m_threadInitCallback;
	AtomicInt32 m_started;
	int32 m_nextConnId;
	ConnectionMap m_mapConnection;
};



#endif
