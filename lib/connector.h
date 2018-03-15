#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "type_define.h"
#include "inet_address.h"
#include "nocopyable.h"

class Channel;
class EventLoop;

class Connector : private Nocopyable, public std::enable_shared_from_this < Connector >
{
public:
	typedef std::function<void(int32 sockfd)> NewConnectionCallback;

	Connector(EventLoop* loop, const InetAddress& servaddr);
	~Connector();

	void SetNewConnectionCallback(const NewConnectionCallback& cb)
	{
		m_newConnectionCallback = cb;
	}

	void Start();
	void Restart();
	void Stop();

	const InetAddress& ServerAddr() const { return m_servAddr; }


private:
	enum CONN_STATE { kDisconnected, kConnecting, kConnected};

	static const int32 kMaxRetryDelayMs = 30 * 1000;
	static const int32 kInitRetryDelayMs = 500;


	void setState(CONN_STATE s) { m_state = s; }
	void startInLoop();
	void stopInLoop();
	void connect();
	void connecting(int32 sockfd);
	void handleWrite();
	void handleError();
	void retry(int32 sockfd);
	int32 removeAndResetChannel();
	void resetChannel();

	EventLoop* m_pLoop;
	InetAddress m_servAddr;
	bool m_bConnect;
	CONN_STATE m_state;
	std::unique_ptr<Channel> m_channel;
	NewConnectionCallback m_newConnectionCallback;
	int32 m_retryDelayMs;
};




#endif