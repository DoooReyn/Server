#include "inet_address.h"
#include "socket_ops.h"
#include "logger.h"


static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

STATIC_ASSERT_CHECK(sizeof(InetAddress) == sizeof(struct sockaddr_in6));
STATIC_ASSERT_CHECK(offsetof(sockaddr_in, sin_family) == 0);
STATIC_ASSERT_CHECK(offsetof(sockaddr_in6, sin6_family) == 0);
STATIC_ASSERT_CHECK(offsetof(sockaddr_in, sin_port) == 2);
STATIC_ASSERT_CHECK(offsetof(sockaddr_in6, sin6_port) == 2);

InetAddress::InetAddress(uint16 port /*= 0*/, bool loopbackonly /*= false*/, bool ipv6 /*= false*/)
{
	STATIC_ASSERT_CHECK(offsetof(InetAddress, m_addr) == 0);
	STATIC_ASSERT_CHECK(offsetof(InetAddress, m_addr6) == 0);
	if (ipv6)
	{
		bzero(&m_addr6, sizeof(m_addr6));
		m_addr6.sin6_family = AF_INET6;
		m_addr6.sin6_addr = loopbackonly ? in6addr_loopback : in6addr_any;
		m_addr6.sin6_port = HostToNetwork16(port);
	}
	else
	{
		bzero(&m_addr, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
		m_addr.sin_addr.s_addr = HostToNetwork32(loopbackonly ? INADDR_LOOPBACK : INADDR_ANY);
		m_addr.sin_port = HostToNetwork16(port);
	}
}

InetAddress::InetAddress(string ip, uint16 port, bool ipv6 /*= false*/)
{
	if (ipv6)
	{
		bzero(&m_addr6, sizeof(m_addr6));
		FromIpPort(ip.c_str(), port, &m_addr6);
	}
	else
	{
		bzero(&m_addr, sizeof(m_addr));
		FromIpPort(ip.c_str(), port, &m_addr);
	}

}

std::string InetAddress::ToIP() const
{
	char buf[64] = { 0 };
	AddrToIP(buf, sizeof(buf), GetSockAddr());
	return buf;
}

std::string InetAddress::ToIPPort() const
{
	char buf[64] = {0};
	AddrToIPPort(buf, sizeof(buf), GetSockAddr());
	return buf;
}

uint16 InetAddress::ToPort() const
{
	return NetworkToHost16(PortNetEndian());
}

uint32 InetAddress::IpNetEndian() const
{
	assert(Family() == AF_INET);
	return m_addr.sin_addr.s_addr;
}

static __thread char t_resolveBuffer[64 * 1024] = {0};

bool InetAddress::Resolve(string hostname, InetAddress* result)
{
	assert(result != NULL);
	struct hostent hent;
	struct hostent* he;
	int32 herrno = 0;
	bzero(&hent, sizeof(hent));

	int32 ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof(t_resolveBuffer), &he, &herrno);
	if (ret == 0 && he != NULL)
	{
		assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
		result->m_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
		return true;
	}
	else
	{
		if (ret)
		{
			ERROR("InetAddress::resolve");
		}
		return false;
	}

}
