#ifndef _GLOBDEF_H_
#define _GLOBDEF_H_

#include "io.h"

#ifndef HAVE_DECL_SETENV
extern int setenv(const char *, const char *, int);
#endif


typedef int bool_t;
#define true 1
#define false 0


#define TRY_ALLOC(res)            \
        if (NULL == ((res)))      \
                error(E_NOMEM, 1);


extern int kcsh_argc;
extern char **kcsh_argv;


/*
 * SIGINT signal handler
 */
void sigint_handler(int signal);


/*
 * Initializes things
 */
void goodmorning(void);


/*
 * Clear resources, reset terminal, etc -- everything here.
 */
void goodnight(int exit_code);

#endif /* _GLOBDEF_H_ */
