#include "connector.h"

#include "logger.h"
#include "channel.h"
#include "event_loop.h"
#include "socket_ops.h"

const int Connector::kMaxRetryDelayMs;
const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& servaddr)
	: m_pLoop(loop)
	, m_servAddr(servaddr)
	, m_bConnect(false)
	, m_state(kDisconnected)
	, m_retryDelayMs(kInitRetryDelayMs)
{
	//DEBUG("Connector ctor[%p]", this);
}

Connector::~Connector()
{
	//DEBUG("Connector dctor[%p]", this);
	assert(!m_channel);
}

void Connector::Start()
{
	m_bConnect = true;
	m_pLoop->RunInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::Restart()
{
	m_pLoop->AssertInLoopThread();
	setState(kDisconnected);
	m_retryDelayMs = kInitRetryDelayMs;
	m_bConnect = true;
	startInLoop();
}

void Connector::Stop()
{
	m_bConnect = false;
	m_pLoop->QueueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::startInLoop()
{
	m_pLoop->AssertInLoopThread();
	assert(m_state == kDisconnected);
	if (m_bConnect)
	{
		connect();
	}
	else
	{
		DEBUG("do not connect");
	}
}

void Connector::stopInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (m_state == kConnecting)
	{
		setState(kDisconnected);
		//removeAndResetChannel();
		int32 sockfd = removeAndResetChannel();
		retry(sockfd);
	}
}

void Connector::connect()
{
	int32 sockfd = CreateNonblockingOrDie(m_servAddr.Family());
	int32 ret = ::Connect(sockfd, m_servAddr.GetSockAddr());
	int32 savedErrno = (ret == 0) ? 0 : errno;
	switch (savedErrno)
	{

		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;

		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
			ERROR("connect error in Connector::startInLoop Errono:%d", savedErrno);
			::Close(sockfd);
			break;

		default:
			ERROR("Unexpected error in Connector::startInLoop Errno:%d", savedErrno);
			::Close(sockfd);
			// connectErrorCallback_();
			break;
	}
}

void Connector::connecting(int32 sockfd)
{
	setState(kConnecting);
	assert(!m_channel);
	m_channel.reset(new Channel(m_pLoop, sockfd));
	m_channel->SetWriteCallBack(std::bind(&Connector::handleWrite, this));
	m_channel->SetErrorCallBack(std::bind(&Connector::handleError, this));
	m_channel->EnableWriting();
	//DEBUG("connecting");

}

void Connector::handleWrite()
{
	//DEBUG("Connector::handleWrite State:%d", m_state);
	if (m_state == kConnecting)
	{
		int32 sockfd = removeAndResetChannel();
		int32 err = GetSocketError(sockfd);
		if (err)
		{
			WARN("Connector::handleWrite - SO_ERROR = %d", err);
		}
		else if (IsSelfConnect(sockfd))
		{
			WARN("Connector::handleWrite -Self connect");
			retry(sockfd);
		}
		else
		{
			setState(kConnected);
			if (m_bConnect)
			{
				m_newConnectionCallback(sockfd);
			}
		}
	}
	else
	{
		assert(m_state == kDisconnected);
	}
}

void Connector::handleError()
{
	ERROR("Connector::handleError state=%d", m_state);
	if (m_state == kConnecting)
	{
		int32 sockfd = removeAndResetChannel();
		int32 err = GetSocketError(sockfd);
		DEBUG("SO_ERROR = %d %s", err, strerror_tl(err));
		retry(sockfd);
	}
}

void Connector::retry(int32 sockfd)
{
	::Close(sockfd);
	setState(kDisconnected);
	if (m_bConnect)
	{
		INFO("Connector::retry -sockfd:%d Retry connecting to %s in %d milliseconds. ", sockfd, m_servAddr.ToIPPort().c_str(), m_retryDelayMs);
		m_pLoop->RunAfter(m_retryDelayMs / 1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
		m_retryDelayMs = std::min(m_retryDelayMs * 2, kMaxRetryDelayMs);
	}
	else
	{
		DEBUG("do not connect");
	}
}

int32 Connector::removeAndResetChannel()
{
	m_channel->DisableAll();
	m_channel->Remove();
	int32 sockfd = m_channel->GetFd();
	m_pLoop->QueueInLoop(std::bind(&Connector::resetChannel, this));
	return sockfd;
}

void Connector::resetChannel()
{
	m_channel.reset();
}



