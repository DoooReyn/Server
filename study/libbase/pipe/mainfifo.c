#include "base.h"

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

void client(int, int);
void server(int, int);

int main()
{
	int		readfd,writefd;
	pid_t	childpid;

	
	if((mkfifo(FIFO1,FILE_MODE) < 0) && errno != EEXIST)
	{
		err_sys("can't create %s",FIFO1);
	}
	
	if((mkfifo(FIFO2,FILE_MODE) < 0) && errno != EEXIST)
	{
		err_sys("can't create %s",FIFO1);
	}


	if ( (childpid = Fork()) == 0)  		/* child */
	{
		readfd = Open(FIFO1,O_RDONLY,0);
		writefd = Open(FIFO2,O_WRONLY,0);

		server(readfd,writefd);
		exit(0);
	}
	/* 4parent */
	writefd = Open(FIFO1,O_WRONLY,0);
	readfd = Open(FIFO2,O_RDONLY,0);

	client(readfd,writefd);

	Waitpid(childpid, NULL, 0);		/* wait for child to terminate */
	
	Close(readfd);
	Close(writefd);
	
	Unlink(FIFO1);
	Unlink(FIFO2);

	return 0;
}
