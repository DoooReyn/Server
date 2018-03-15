#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <iostream>

using namespace std;

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)


int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{
		ERR_EXIT("socket error");
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		ERR_EXIT("connect error");
	}

	struct sockaddr_in localaddr;
	socklen_t addrlen = sizeof(localaddr);
	ret = getsockname(sock, (struct sockaddr*)&localaddr, &addrlen);
	if (ret < 0)
	{
		ERR_EXIT("getsockname error");
	}

	std::cout << "ip=" << inet_ntoa(localaddr.sin_addr) <<
	          " port=" << ntohs(localaddr.sin_port) << std::endl;

	char sendbuf[1024] = {0};
	char recvbuf[1024] = {0};

	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{
		write(sock, sendbuf, strlen(sendbuf));
		read(sock, recvbuf, sizeof(recvbuf));

		fputs(recvbuf, stdout);
		memset(sendbuf, 0, sizeof(sendbuf));
		memset(recvbuf, 0, sizeof(recvbuf));
	}
	return 0;
}
