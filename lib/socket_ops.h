#ifndef __SOCKET_OPS_H__
#define __SOCKET_OPS_H__

#include "type_define.h"
#include "nocopyable.h"
#include "inet_address.h"

inline uint64 HostToNetwork64(uint64 host64)
{
	return htobe64(host64);
}

inline uint32 HostToNetwork32(uint32 host32)
{
	return htobe32(host32);
}

inline uint16 HostToNetwork16(uint16 host16)
{
	return htobe16(host16);
}

inline uint64 NetworkToHost64(uint64 net64)
{
	return be64toh(net64);
}

inline uint32 NetworkToHost32(uint32 net32)
{
	return be32toh(net32);
}

inline uint16 NetworkToHost16(uint16 net16)
{
	return be16toh(net16);
}
void FromIpPort(const char* ip, uint16 port, struct sockaddr_in* addr);
void FromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);
void AddrToIPPort(char* buf, size_t size, const struct sockaddr* addr);
void AddrToIP(char* buf, size_t size, const struct sockaddr* addr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
int32 CreateNonblockingOrDie(uint16 family);
int32 BindOrDie(int32 sockfd, const struct sockaddr* addr);
int Connect(int sockfd, const struct sockaddr* addr);
ssize_t Read(int sockfd, void* buf, size_t count);
ssize_t Readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t Write(int sockfd, const void* buf, size_t count);
void Close(int sockfd);
void ShutdownWrite(int sockfd);
int32 GetSocketError(int32 sockfd);
struct sockaddr_in6 GetLocalAddr(int sockfd);
struct sockaddr_in6 GetPeerAddr(int sockfd);
int32 Accept(int32 sockfd, struct sockaddr_in6* addr);
bool IsSelfConnect(int sockfd);

//=========================================

class Socket : private Nocopyable
{
public:
	explicit Socket(int32 sockfd) : m_sockfd(sockfd)
	{}
	~Socket();

	int GetFd() const { return m_sockfd; }

	bool GetTcpInfo(struct tcp_info* tcp_info) const;
	bool GetTcpInfoString(char* buf, int32 len) const;

	bool BindAddress(const InetAddress& localaddr);

	void Listen();

	int32 Accept(InetAddress* peeraddr);

	void ShutdownWrite();

	void SetTcpNoDelay(bool on);

	void SetReuseAddr(bool on);

	void SetReusePort(bool on);

	void SetKeepAlive(bool on);


private:
	const int32 m_sockfd;
};




















#endif
