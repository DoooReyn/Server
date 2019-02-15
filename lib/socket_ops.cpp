#include "socket_ops.h"
#include "logger.h"



typedef struct sockaddr SA;

const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr)
{
	return static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr));
}

const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr)
{
	return static_cast<const struct sockaddr_in6*>(static_cast<const void*>(addr));
}


const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr)
{
	return static_cast<const struct sockaddr*>(static_cast<const void*>(addr));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr)
{
	return static_cast<struct sockaddr*>(static_cast<void*>(addr));
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr)
{
	return static_cast<const struct sockaddr*>(static_cast<const void*>(addr));
}


int32 CreateNonblockingOrDie(uint16 family)
{
	int32 socket = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (socket < 0)
	{
		ERROR("socket create error");
		abort();
	}
	return socket;
}

int32 BindOrDie(int32 sockfd, const struct sockaddr* addr)
{
	int32 ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
	if (ret  < 0)
	{
		ERROR("bind error");
	}
	return ret;
}

void ListenOrDie(int32 sockfd)
{
	int32 ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0)
	{
		ERROR("listen error");
		abort();
	}
}

int32 Accept(int32 sockfd, struct sockaddr_in6* addr)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
	int32 connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	//WARN("Accept fd:%d", connfd);
	if (connfd < 0)
	{
		int32 savedErrno = errno;
		ERROR("sock accept4");
		switch (savedErrno)
		{
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO: // ???
			case EPERM:
			case EMFILE: // per-process lmit of open file desctiptor ???
				// expected errors
				errno = savedErrno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				// unexpected errors
				ERROR("unexpected error of ::accept ");
				abort();
				break;
			default:
				ERROR("unknown error of ::accept %d", savedErrno);
				abort();
				break;
		}
	}
	return connfd;
}


int Connect(int sockfd, const struct sockaddr* addr)
{
	return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

ssize_t Read(int sockfd, void* buf, size_t count)
{
	return ::read(sockfd, buf, count);
}

ssize_t Readv(int sockfd, const struct iovec* iov, int iovcnt)
{
	return ::readv(sockfd, iov, iovcnt);
}

ssize_t Write(int sockfd, const void* buf, size_t count)
{
	return ::write(sockfd, buf, count);
}

void Close(int sockfd)
{
	if (::close(sockfd) < 0)
	{
		ERROR("close");
	}
}

void ShutdownWrite(int sockfd)
{
	if (::shutdown(sockfd, SHUT_WR) < 0)
	{
		ERROR("sockets::shutdownWrite");
	}
}

void AddrToIP(char* buf, size_t size, const struct sockaddr* addr)
{
	if (addr->sa_family == AF_INET)
	{
		assert(size >= INET_ADDRSTRLEN);
		const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
		::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
	}
	else if (addr->sa_family == AF_INET6)
	{
		assert(size >= INET6_ADDRSTRLEN);
		const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
		::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
	}
}

void AddrToIPPort(char* buf, size_t size, const struct sockaddr* addr)
{
	AddrToIP(buf, size, addr);
	size_t end = ::strlen(buf);
	const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
	uint16 port = NetworkToHost16(addr4->sin_port);
	assert(size > end);
	snprintf(buf + end, size - end, ":%u", port);
}


void FromIpPort(const char* ip, uint16 port, struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = HostToNetwork16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		ERROR("FromIpPort IP:%s Port:%d", ip, port);
	}
}

void FromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr)
{
	addr->sin6_family = AF_INET6;
	addr->sin6_port = HostToNetwork16(port);
	if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
	{
		ERROR("FromIpPort IP:%s Port:%d", ip, port);
	}
}

int32 GetSocketError(int32 sockfd)
{
	int32 optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
	{
		return errno;
	}
	else
	{
		return optval;
	}
}

struct sockaddr_in6 GetLocalAddr(int sockfd)
{
	struct sockaddr_in6 localaddr;
	bzero(&localaddr, sizeof localaddr);
	socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
	if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
	{
		ERROR("sockets::getLocalAddr");
	}
	return localaddr;
}


struct sockaddr_in6 GetPeerAddr(int sockfd)
{
	struct sockaddr_in6 peeraddr;
	bzero(&peeraddr, sizeof peeraddr);
	socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
	if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
	{
		ERROR("sockets::getPeerAddr");
	}
	return peeraddr;
}

bool IsSelfConnect(int sockfd)
{
	struct sockaddr_in6 localaddr = GetLocalAddr(sockfd);
	struct sockaddr_in6 peeraddr = GetPeerAddr(sockfd);
	if (localaddr.sin6_family == AF_INET)
	{
		const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
		const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
		return laddr4->sin_port == raddr4->sin_port
		       && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
	}
	else if (localaddr.sin6_family == AF_INET6)
	{
		return localaddr.sin6_port == peeraddr.sin6_port
		       && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
	}
	else
	{
		return false;
	}
}

//============================================================

Socket::~Socket()
{
	Close(m_sockfd);
}

bool Socket::GetTcpInfo(struct tcp_info* tcpi) const
{
	socklen_t len = sizeof(*tcpi);
	bzero(tcpi, len);
	return ::getsockopt(m_sockfd, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::GetTcpInfoString(char* buf, int32 len) const
{
	struct tcp_info tcpi;
	bool ok = GetTcpInfo(&tcpi);
	if (ok)
	{
		snprintf(buf, len, "unrecovered=%u "
		         "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
		         "lost=%u retrans=%u rtt=%u rttvar=%u "
		         "sshthresh=%u cwnd=%u total_retrans=%u",
		         tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
		         tcpi.tcpi_rto,          // Retransmit timeout in usec
		         tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
		         tcpi.tcpi_snd_mss,
		         tcpi.tcpi_rcv_mss,
		         tcpi.tcpi_lost,         // Lost packets
		         tcpi.tcpi_retrans,      // Retransmitted packets out
		         tcpi.tcpi_rtt,          // Smoothed round trip time in usec
		         tcpi.tcpi_rttvar,       // Medium deviation
		         tcpi.tcpi_snd_ssthresh,
		         tcpi.tcpi_snd_cwnd,
		         tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
	}
	return ok;
}

bool Socket::BindAddress(const InetAddress& localaddr)
{
	int32 ret = BindOrDie(m_sockfd, localaddr.GetSockAddr());
	if(ret < 0)
	{
		return false;
	}
	return true;
}

void Socket::Listen()
{
	ListenOrDie(m_sockfd);
}

int32 Socket::Accept(InetAddress* peeraddr)
{
	struct sockaddr_in6 addr;
	bzero(&addr, sizeof(addr));
	int32 connfd = ::Accept(m_sockfd, &addr);
	if (connfd > 0)
	{
		peeraddr->SetSockAddrInet6(addr);
	};
	return connfd;
}

void Socket::ShutdownWrite()
{
	::ShutdownWrite(m_sockfd);
}

void Socket::SetTcpNoDelay(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::SetReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::SetReusePort(bool on)
{

#ifdef SO_REUSEPORT
	int optval = on ? 1 : 0;
	int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
	if (ret < 0 && on)
	{
		ERROR("SO_REUSEPORT failed");
	}
#else
	if (on)
	{
		ERROR("SO_REUSEPORT is not supported.");
	}
#endif

}

void Socket::SetKeepAlive(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
}

