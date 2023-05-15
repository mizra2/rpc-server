CC=cc
RPC_SYSTEM=rpc.o
UTIL=util.o
SERVER=rpc-server
CLIENT=rpc-client

.PHONY: format all

all: $(RPC_SYSTEM) $(UTIL) $(SERVER) $(CLIENT)

$(RPC_SYSTEM): rpc.c rpc.h
	$(CC) -c -o $@ $<

$(UTIL): util.c util.h
	$(CC) -c -o $@ $<

$(SERVER): server.c $(RPC_SYSTEM) $(UTIL)
	$(CC) -Wall -o $@ $^

$(CLIENT): client.c $(RPC_SYSTEM) $(UTIL)
	$(CC) -Wall -o $@ $^

format:
	clang-format -style=file -i *.c *.h

clean:
	rm -f rpc.o util.o

