# Made by ValeriyKr #

ifdef GCC
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

all: kcsh.o io.o parser.o
	$(CC) $(CFLAGS) kcsh.o parser.o io.o -o kcsh

kcsh.o: kcsh.c globdef.h io.h command.h
	$(CC) $(CFLAGS) -c kcsh.c -o kcsh.o

io.o: io.c globdef.h io.h
	$(CC) $(CFLAGS) -c io.c -o io.o

parser.o: parser.c globdef.h parser.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

clean:
	rm -f ./*.o

run: all
	./kcsh
