CC = gcc
CFLAGS = -g -O2 -D_REENTRANT -Wall
LIBS = ../libunpipc.a -lrt -lpthread
PROGS = mainpipe mainpopen mainfifo

CLEANFILES = core core.* *.core *.o temp.* *.out


all:${PROGS}

mainpipe:mainpipe.o client.o server.o
	$(CC) $(CFLAGS) -o $@ $^ ${LIBS}

mainpopen:mainpopen.o
	 $(CC) $(CFLAGS) -o $@ $^ ${LIBS}

mainfifo:mainfifo.o client.o server.o
	$(CC) $(CFLAGS) -o $@ $^ ${LIBS}

clean:
	rm -f $(PROGS) $(CLEANFILES)

