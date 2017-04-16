#ifndef _GLOBDEF_H_
#define _GLOBDEF_H_

typedef int bool_t;
#define true 1
#define false 0


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
