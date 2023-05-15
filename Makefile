CC=cc
RPC_SYSTEM=rpc.o
SERVER=rpc-server
CLIENT=rpc-client
LIBRARY=librpc.a

.PHONY: format all

all: $(LIBRARY) $(SERVER) $(CLIENT)

$(RPC_SYSTEM): rpc.c rpc.h
	$(CC) -c -o $@ $<


$(SERVER): server.c $(LIBRARY)
	$(CC) -Wall -L. -o $@ $^ -lrpc

$(CLIENT): client.c $(LIBRARY)
	$(CC) -Wall -L. -o $@ $^ -lrpc

format:
	clang-format -style=file -i *.c *.h

clean:
	rm -f rpc.o
