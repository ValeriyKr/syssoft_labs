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

all: server client multithread multithread6 multithread7 unixserver \
		unixclient signalserver pipes

server: server.o
	$(CC) $(CFLAGS) server.o -o server
	cp server shmserver
	cp server msgserver
	cp server mmapserver

server.o: server.c state.h
	$(CC) $(CFLAGS) -c server.c -o server.o
	
client: client.o
	$(CC) $(CFLAGS) client.o -o client

client.o: client.c state.h
	$(CC) $(CFLAGS) -c client.c -o client.o

multithread: multithread.o
	$(CC) $(CFLAGS) multithread.o -o multithread
	cp multithread multithread4
	cp multithread multithread5

multithread.o: multithread.c
	$(CC) $(CFLAGS) -c multithread.c -o multithread.o

multithread6: multithread6.o
	$(CC) $(CFLAGS) multithread6.o -o multithread6

multithread6.o: multithread6.c
	$(CC) $(CFLAGS) -c multithread6.c -o multithread6.o

multithread7: multithread7.o
	$(CC) $(CFLAGS) multithread7.o -o multithread7

multithread7.o: multithread7.c
	$(CC) $(CFLAGS) -c multithread7.c -o multithread7.o

unixserver: unixserver.o
	$(CC) $(CFLAGS) unixserver.o -o unixserver

unixserver.o: unixserver.c state.h
	$(CC) $(CFLAGS) -c unixserver.c -o unixserver.o

unixclient: unixclient.o
	$(CC) $(CFLAGS) unixclient.o -o unixclient

unixclient.o: unixclient.c state.h
	$(CC) $(CFLAGS) -c unixclient.c -o unixclient.o

signalserver: signalserver.o
	$(CC) $(CFLAGS) signalserver.o -o signalserver

signalserver.o: signalserver.c state.h
	$(CC) $(CFLAGS) -c signalserver.c -o signalserver.o

pipes: pipes.o
	$(CC) $(CFLAGS) pipes.o -o pipes

pipes.o: pipes.c state.h
	$(CC) $(CFLAGS) -c pipes.c -o pipes.o

clean:
	rm -f ./*.o

srun: server
	./server

crun: client
	./client
