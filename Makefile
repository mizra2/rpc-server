CC=cc
RPC_SYSTEM=rpc.o
LIBRARY=librpc.a
SERVER=rpc-server
CLIENT=rpc-client

.PHONY: format all

all: $(LIBRARY) $(SERVER) $(CLIENT)

$(RPC_SYSTEM): rpc.c rpc.h
	$(CC) -Wall -pthread -c -o $@ $<

$(LIBRARY): $(RPC_SYSTEM)
	ar rcs $@ $^

$(SERVER): server.c $(LIBRARY)
	$(CC) -Wall -pthread -o $@ $^

$(CLIENT): client.c $(LIBRARY)
	$(CC) -Wall -pthread -o $@ $^

format:
	clang-format -style=file -i *.c *.h

clean:
	rm -f $(RPC_SYSTEM) $(LIBRARY) $(SERVER) $(CLIENT)