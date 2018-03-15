#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>


#define MAXEPOLLSIZE 10000
#define MAXLINE 10240

int setnonblocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	int nRet = fcntl(sockfd, F_SETFL, flags);
	return nRet;
}


int test_epoll(int argc, char const* argv[])
{
	char buf[MAXLINE];

	int nPort = 9999;
	int nListen = 1024;
	int ListenFd = 0;
	int ConnFd = 0;

	struct sockaddr_in servaddr, cliaddr;
	struct epoll_event ev;
	struct epoll_event events[MAXEPOLLSIZE];

	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&servaddr, 0, addrlen);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(nPort);

	ListenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(ListenFd == -1)
	{
		perror("can't create socket file");
		return -1;
	}

	int opt = 1;
	setsockopt(ListenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(setnonblocking(ListenFd) < 0)
	{
		perror("setnonblock error");
	}

	if(bind(ListenFd, (struct sockaddr*) &servaddr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind error");
		return -1;
	}

	if(listen(ListenFd, nListen) == -1)
	{
		perror("listen error");
		return -1;
	}

	int kdpfd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN;
	ev.data.fd = ListenFd;
	epoll_ctl(kdpfd, EPOLL_CTL_ADD, ListenFd, &ev);

	while(true)
	{
		int nfds = epoll_wait(kdpfd, events, MAXEPOLLSIZE, 0);
		for(int i = 0; i < nfds; i++)
		{
			int sock_fd = events[i].data.fd;
			if(sock_fd == ListenFd)
			{
				ConnFd = accept(ListenFd, (struct sockaddr*)&cliaddr, &addrlen);
				sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
				printf("%s", buf);

				if(setnonblocking(ConnFd) < 0)
				{
					perror("setnonblocking error");
				}
				ev.events = EPOLLIN | EPOLLOUT;
				ev.data.fd = ConnFd;
				epoll_ctl(kdpfd, EPOLL_CTL_ADD, ConnFd, &ev);
			}
			else if(events[i].events & EPOLLIN)
			{
				memset(buf, 0, sizeof(buf));
				int retcode = TEMP_FAILURE_RETRY(::recv(sock_fd, buf, MAXLINE, MSG_NOSIGNAL));
				if(retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
				{
					printf("recv try again\n");
					continue;
				}
				printf("recv :%s\n", buf);
				strcat(buf, " OK !");
			}
			else if(events[i].events & EPOLLOUT)
			{
				int retcode =  TEMP_FAILURE_RETRY(::send(sock_fd, buf, strlen(buf), MSG_NOSIGNAL));
				if(retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
				{
					printf("send try again\n");
					continue;
				}
			}
		}
	}
	return 0;
}
