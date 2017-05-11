#ifndef _IO_H_
#define _IO_H_

#include <unistd.h>

/**
 * \file io.h
 * \author kk
 *
 * Functions for I/O
 */

/**
 * \def IN
 * Alias for STDIN_FILENO
 */
#define IN STDIN_FILENO
/**
 * \def OUT
 * Alias for STDOUT_FILENO
 */
#define OUT STDOUT_FILENO
/**
 * \def ERR
 * Alias for STDERR_FILENO
 */
#define ERR STDERR_FILENO


/**
 * \enum error_in
 *
 * Enumeration of error reasons. Used by \code{ error() } function
 */
enum error_in {
        E_NOTERM,
        E_NOMEM,
        E_BREG,
        E_WRITE,
        E_READ,
        E_MANYARGS,
        E_SYNTAX,
        E_INVARGS,
        E_FORK,
        E_DUP,
        E_PIPE,
        E_EXEC,
        E_UNDEF
};


/**
 * Initializes terminal.
 */
void init_term(void);


/**
 * Resets terminal to default mode.
 */
void reset_term(void);


/**
 * Says something to stdout.
 *
 * \param message message to say.
 */
void say(const char *message);


/**
 * Says char to stdout.
 *
 * \param c symbol to say.
 */
void sayc(char c);


/**
 * Says something to stdout, ends it with '\n'.
 *
 * \param message message to say.
 */
void sayln(const char *message);


/**
 * Says int to stdout.
 *
 * \param n number to say.
 */
void sayi(int n);


/**
 * Says unsignad long  to stdout.
 *
 * \param n number to say.
 */
void sayul(unsigned long n);


/**
 * Returns string representation of int, which is error code.
 *
 * \param n integer in range [0; 256).
 * \return pointer to string.
 */
char * errc_to_a(int n);


/**
 * Prints error to stderr and exits if exit is required.
 * Use assert(3C) to check correctness of functions' arguments
 * and allocation's success.
 *
 * \param subj subkect of error
 * \param do_exit ono-zero if want to exit after this error
 */
void error(enum error_in subj, int do_exit);


/**
 * Interacts with users and returns their input at hte end.
 *
 * \return user input to parse and execute.
 */
char* get_line(void);


#endif /* _IO_H_ */
