# Made by ValeriyKr #
CC = clang
CFLAGS = --std=c89 -Weverything -Wall -Wextra -Wpedantic -O2 \
		 -Wno-reserved-id-macro

all: kcsh.o parser.o
	$(CC) $(CFLAGS) kcsh.o parser.o -o kcsh

kcsh.o:
	$(CC) $(CFLAGS) -c kcsh.c -o kcsh.o

parser.o:
	$(CC) $(CFLAGS) -c parser.c -o parser.o
	
clean:
	rm -f ./*.o
