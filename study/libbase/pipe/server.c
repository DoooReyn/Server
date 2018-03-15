#include "base.h"

void server(int readfd, int writefd)
{
	int fd = 0;
	ssize_t n = 0;
	char buff[MAXLINE + 1] = {0};

	printf("1 ============================\n");

	if ((n = Read(readfd, buff, MAXLINE)) == 0)
	{
		err_quit("end of file while reading pathname");
	}
	buff[n] = '\0';
	if((fd = open(buff, O_RDONLY)) < 0)
	{
		snprintf(buff + n, sizeof(buff) - n, ":can't open,%s\n", strerror(errno));
		n = strlen(buff);
		Write(writefd, buff, n);
	}
	else
	{

		printf("2 ============================\n");
		while( ( n = Read(fd, buff, MAXLINE)) > 0)
		{
			printf("3 =====================================\n");
			Write(writefd, buff, n);
		}

		Close(fd);
	}
}
