CC=cc
RPC_SYSTEM=rpc.o
UTIL=util.o

.PHONY: format all

all: $(RPC_SYSTEM) $(UTIL)

$(RPC_SYSTEM): rpc.c rpc.h
	$(CC) -c -o $@ $<

$(UTIL): util.c util.h
	$(CC) -c -o $@ $<

# RPC_SYSTEM_A=rpc.a
# $(RPC_SYSTEM_A): rpc.o
# 	ar rcs $(RPC_SYSTEM_A) $(RPC_SYSTEM)

format:
	clang-format -style=file -i *.c *.h
