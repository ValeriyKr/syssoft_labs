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

all: todo kcsh.o io.o parser.o builtin.o builtins_impl.o variable.o job.o
	$(CC) $(CFLAGS) kcsh.o parser.o io.o builtin.o builtins_impl.o variable.o \
			job.o -o kcsh

kcsh.o: kcsh.c globdef.h io.h command.h parser.h builtin.h builtins_impl.h \
		job.h
	$(CC) $(CFLAGS) -c kcsh.c -o kcsh.o

io.o: io.c globdef.h io.h
	$(CC) $(CFLAGS) -c io.c -o io.o

parser.o: parser.c globdef.h parser.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

builtin.o: builtin.c globdef.h builtin.h
	$(CC) $(CFLAGS) -c builtin.c -o builtin.o

builtins_impl.o: builtins_impl.c builtins_impl.h globdef.h io.h variable.h
	$(CC) $(CFLAGS) -c builtins_impl.c -o builtins_impl.o

variable.o: variable.c globdef.h variable.h io.h
	$(CC) $(CFLAGS) -c variable.c -o variable.o

job.o: job.c job.h
	$(CC) $(CFLAGS) -c job.c -o job.o

doc: Doxyfile *.h *.c
	doxygen

doc_tex: doc
	cd latex && make && cd ..

check_codestyle: *.c *.h
	@echo ' >> Long lines:'
	@egrep '.{80,}' *.c *.h
	@echo ' >> Codestyle checking end'

todo:
	@echo "\tTODO list:"
	@echo "\t\t1. IO redirections" 
	@echo "\n\tTODO marks in sources:\n"
	@find . \( -name '*.h' -o -name '*.c' \) -exec grep TODO {} \;

clean:
	rm -f ./*.o

run: all
	./kcsh
