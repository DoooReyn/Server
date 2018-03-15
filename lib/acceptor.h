#ifndef __ACCEPT_H__
#define __ACCEPT_H__

#include "nocopyable.h"
#include "channel.h"
#include "socket_ops.h"

class EventLoop;
class InetAddress;

class Acceptor : private Nocopyable
{
public:
	typedef std::function<void(int32 sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop* loop, const InetAddress& listen_addr, bool reuseport);

	~Acceptor();

	void SetNewConnectionCallback(const NewConnectionCallback& cb)
	{
		m_newConnectionCallback = cb;
	}

	bool Listening() const { return m_bListening; }
	bool Binded() const { return m_bBinded; }

	void Listen();

private:
	void handleRead();
	EventLoop* m_pLoop;
	Socket m_acceptSock;
	Channel m_acceptChannel;
	NewConnectionCallback m_newConnectionCallback;
	bool m_bListening;
	bool m_bBinded;
	int32 m_idleFd;
};


#endif