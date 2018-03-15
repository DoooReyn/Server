#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__

#include "type_define.h"

class InetAddress
{
public:
	explicit InetAddress(uint16 port = 0, bool loopbackonly = false, bool ipv6 = false);

	InetAddress(string ip, uint16 port, bool ipv6 = false);

	explicit InetAddress(const struct sockaddr_in& addr) : m_addr(addr)
	{}

	explicit InetAddress(const struct sockaddr_in6& addr) : m_addr6(addr)
	{}

	int16 Family() const { return m_addr.sin_family; }

	std::string ToIP() const;

	std::string ToIPPort() const;

	uint16 ToPort() const;



	const struct sockaddr* GetSockAddr() const
	{
		return (struct sockaddr*)(&m_addr);
	}

	void SetSockAddrInet6(const struct sockaddr_in6& addr6)
	{
		m_addr6 = addr6;
	}

	uint32 IpNetEndian() const;
	uint32 PortNetEndian() const
	{
		return m_addr.sin_port;
	}

	static bool Resolve(string hostname, InetAddress* result);

private:
	union
	{
		struct sockaddr_in m_addr;
		struct sockaddr_in6 m_addr6;
	};
};



#endif