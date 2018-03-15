CC=g++
CPPFLAGS=-Wall -Werror -g
BIN=svr_poll
CLIENT=client

all:$(BIN) $(CLIENT)

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

$(BIN):poll_server.o
	$(CC) -o $@ $^

$(CLIENT):client.o
	$(CC) -o $@ $^


clean:
	rm -f *.o $(BIN) $(CLIENT)
