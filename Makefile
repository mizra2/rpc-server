CC=cc
RPC_SYSTEM=rpc.o
UTIL=util.o
SERVER=rpc-server
CLIENT=rpc-client
LIBRARY=librpc.a

.PHONY: format all

all: $(LIBRARY) $(SERVER) $(CLIENT)

$(RPC_SYSTEM): rpc.c rpc.h
	$(CC) -c -o $@ $<

$(UTIL): util.c util.h
	$(CC) -c -o $@ $<

$(LIBRARY): $(RPC_SYSTEM) $(UTIL)
	ar rcs $@ $^

$(SERVER): server.c $(LIBRARY)
	$(CC) -Wall -o $@ $^

$(CLIENT): client.c $(LIBRARY)
	$(CC) -Wall -o $@ $^

format:
	clang-format -style=file -i *.c *.h

clean:
	rm -f rpc.o util.o

