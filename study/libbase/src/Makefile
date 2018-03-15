CC = gcc
CFLAGS = -g -O2 -D_REENTRANT -Wall
LIBS = ../libunpipc.a -lrt -lpthread
LIBS_RPC =
RANLIB = ranlib
RPCGEN_OPTS = -C


LIBUNPIPC_NAME = ../libunpipc.a

LIB_OBJS = wrapstdio.o error.o

CLEANFILES = core core.* *.core *.o temp.* *.out


all:${LIB_OBJS}
	ar rv ${LIBUNPIPC_NAME} $?
	${RANLIB} ${LIBUNPIPC_NAME}
clean:
	rm -f $(CLEANFILES)

