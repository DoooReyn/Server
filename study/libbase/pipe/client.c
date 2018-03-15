#include "base.h"

void client(int readfd, int writefd)
{
	size_t len = 0;
	ssize_t n = 0;
	char buff[MAXLINE] = {0};

	//读取一行数据
	Fgets(buff, MAXLINE, stdin);
	len = strlen(buff);
	if (buff[len - 1] == '\n')
	{
		len--;
	}
	Write(writefd, buff, len);

	printf("=====================\n");

	while((n = Read(readfd, buff, MAXLINE)) > 0)
	{
		printf("read n:%ld\n",n);
		Write(STDOUT_FILENO, buff, n);
	}
}
