# Made by ValeriyKr #

# Uncomment next line to use gcc as compiler
#USE_GCC = true

ifdef USE_GCC
	CC = gcc
else
	CC = clang
endif

ifeq ($(CC), clang)
		CFLAGS = --std=gnu89 -Wall -Wextra -pedantic -O0 -g \
			-Weverything -Wno-reserved-id-macro -lrt -pthreads
else
		CFLAGS = --std=gnu89 -Wall -Wextra -pedantic -O0 -g -lrt -pthreads
endif

all: server

server: server.o queue.o
	$(CC) $(CFLAGS) server.o queue.o -o server

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o

clean:
	rm -f ./*.o

