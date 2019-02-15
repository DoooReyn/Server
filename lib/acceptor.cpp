#include "acceptor.h"
#include "event_loop.h"
#include "inet_address.h"
#include "logger.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listen_addr, bool reuseport)
	: m_pLoop(loop)
	, m_acceptSock(::CreateNonblockingOrDie(listen_addr.Family()))
	, m_acceptChannel(loop, m_acceptSock.GetFd())
	, m_bListening(false)
	, m_bBinded(false)
	, m_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(m_idleFd >= 0);
	m_acceptSock.SetReuseAddr(true);
	m_acceptSock.SetReusePort(reuseport);
	m_bBinded = m_acceptSock.BindAddress(listen_addr);
	m_acceptChannel.SetReadCallBack(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
	m_acceptChannel.DisableAll();
	m_acceptChannel.Remove();
	::close(m_idleFd);
}

void Acceptor::Listen()
{
	m_pLoop->AssertInLoopThread();
	m_bListening = true;
	m_acceptSock.Listen();
	m_acceptChannel.EnableReading();
}

void Acceptor::handleRead()
{
	m_pLoop->AssertInLoopThread();
	InetAddress peerAddr;
	int32 connfd = m_acceptSock.Accept(&peerAddr);
	if (connfd > 0)
	{
		if (m_newConnectionCallback)
		{
			m_newConnectionCallback(connfd, peerAddr);
		}
		else
		{
			Close(connfd);
		}
	}
	else
	{
		ERROR("in Acceptor::handleRead");
		if (errno == EMFILE)
		{
			::close(m_idleFd);
			m_idleFd = ::accept(m_acceptSock.GetFd(), nullptr, nullptr);
			::close(m_idleFd);
			m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}
