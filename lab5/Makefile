# Made by ValeriyKr #

# Uncomment next line to use gcc as compiler
#USE_GCC = true

ifdef USE_GCC
	CC = gcc
else
	CC = clang
endif

ifeq ($(CC), clang)
		CFLAGS = --std=c89 -Wall -Wextra -pedantic -O0 -g \
			-Weverything -Wno-reserved-id-macro
else
		CFLAGS = --std=c89 -Wall -Wextra -pedantic -O0 -g
endif

all: server client

server: server.o
	$(CC) $(CFLAGS) server.o -o server

server.o: server.c state.h
	$(CC) $(CFLAGS) -c server.c -o server.o
	
client: client.o
	$(CC) $(CFLAGS) client.o -o client

client.o: client.c state.h
	$(CC) $(CFLAGS) -c client.c -o client.o

clean:
	rm -f ./*.o

srun: server
	./server

crun: client
	./client
