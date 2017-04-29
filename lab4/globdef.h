#ifndef _GLOBDEF_H_
#define _GLOBDEF_H_

#include <sys/types.h>

#include "io.h"

/**
 * \file globdef.h
 * \author kk
 *
 * Structures, defines, global variables and functions, which must be
 * accessible in each point of sources.
 */

#ifndef HAVE_DECL_SETENV
extern int setenv(const char *, const char *, int);
#endif


typedef int bool_t;
#define true 1
#define false 0


/**
 * \def TRU_ALLOC(res)
 *
 * Wrapper which can be used to check success of allocation.
 * Very useful.
 */
#define TRY_ALLOC(res)            \
        if (NULL == ((res)))      \
                error(E_NOMEM, 1);


/**
 * \var kcsh_argc
 * Command line arguments' count
 */
extern int kcsh_argc;

/**
 * \var kcsh_argv
 * Command line arguments
 */
extern char **kcsh_argv;

/**
 * \var kcsh_pid
 * Process ID (PID) of the programme
 */
extern pid_t kcsh_pid;


/**
 * SIGINT signal handler
 */
void sigint_handler(int signal);


/**
 * Initializes things
 */
void goodmorning(void);


/**
 * Clear resources, reset terminal, etc -- everything here.
 */
void goodnight(int exit_code);

#endif /* _GLOBDEF_H_ */
