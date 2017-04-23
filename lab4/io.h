#ifndef _IO_H_
#define _IO_H_

#include <unistd.h>

#define IN STDIN_FILENO
#define OUT STDOUT_FILENO
#define ERR STDERR_FILENO


enum error_in {
        E_NOTERM,
        E_NOMEM,
        E_WRITE,
        E_READ,
        E_MANYARGS,
        E_SYNTAX,
        E_FORK,
        E_EXEC,
        E_UNDEF
};


/*
 * Initializes terminal.
 */
void init_term(void);


/*
 * Resets terminal to default mode.
 */
void reset_term(void);


/*
 * Says something to stdout
 *
 * \param message message to say
 */
void say(const char *message);


/*
 * Says something to stdout, ends it with '\n'
 *
 * \param message message to say
 */
void sayln(const char *message);


/*
 * Says int to stdout.
 *
 * \param n number to say.
 */
void sayi(int n);


/*
 * Says unsignad long  to stdout.
 *
 * \param n number to say.
 */
void sayul(unsigned long n);


/*
 * Prints error to stderr and exits if exit is required.
 * Use assert(3C) to check correctness of functions' arguments
 * and allocation's success.
 *
 * \param subj subkect of error
 * \param do_exit ono-zero if want to exit after this error
 */
void error(enum error_in subj, int do_exit);


/*
 * Interacts with users and returns their input at hte end.
 *
 * \return user input to parse and execute.
 */
char* get_line(void);


#endif /* _IO_H_ */
