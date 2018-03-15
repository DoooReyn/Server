#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <error.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)

typedef std::vector<struct pollfd> vecPollFd;

int main()
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (listenfd < 0)
	{
		ERR_EXIT("socket error!");
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret < 0)
	{
		ERR_EXIT("setsockopt error");
	}

	ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		ERR_EXIT("bind error");
	}

	ret = listen(listenfd, SOMAXCONN);
	if (ret < 0)
	{
		ERR_EXIT("listen error");
	}

	struct pollfd pfd;
	pfd.fd = listenfd;
	pfd.events = POLLIN;

	vecPollFd  vecPolls;

	vecPolls.push_back(pfd);

	int nReady = 0;
	struct sockaddr_in peeraddr;
	socklen_t peerlen;
	int connfd;

	while (1)
	{
		nReady = poll(&*vecPolls.begin(), vecPolls.size(), -1);
		if  (nReady == -1)
		{
			if(errno == EINTR)
			{
				continue;
			}
			ERR_EXIT("poll error");
		}
		if (nReady == 0)
		{
			continue;
		}

		if (vecPolls[0].revents & POLLIN)
		{
			peerlen = sizeof (peeraddr);
			connfd = accept4(listenfd, (struct sockaddr*)&peeraddr, &peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
			if (connfd == -1)
			{
				ERR_EXIT("accept4 error");
			}

			pfd.fd = connfd;
			pfd.events = POLLIN;
			pfd.revents = 0;
			vecPolls.push_back(pfd);
			--nReady;

			std::cout << "ip=" << inet_ntoa(peeraddr.sin_addr) <<
			          " port=" << ntohs(peeraddr.sin_port) << std::endl;
			if (nReady == 0)
			{
				continue;
			}
		}

		for (vecPollFd::iterator it = vecPolls.begin() + 1;
		        it != vecPolls.end() && nReady > 0 ; ++it)
		{
			if (it->revents & POLLIN)
			{
				--nReady;
				connfd = it->fd;
				char buf[1024] = {0};
				int ret = read(connfd, buf, 1024);
				cout << ret << endl;
				if (ret == -1)
				{
					ERR_EXIT("read");
				}
				if (ret == 0)
				{
					std::cout << "client close" << std::endl;
					it = vecPolls.erase(it);
					--it;

					close(connfd);
					continue;
				}

				cout << buf << endl;
				write(connfd, buf, strlen(buf));
			}
		}
	}
	return 0;
}
