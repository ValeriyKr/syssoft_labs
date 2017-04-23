#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <termios.h>
#include <sys/types.h>

#include "globdef.h"


struct cmd {
    int argc;     /* Arguments count                                */
    char **argv;  /* Arguments                                      */
    pid_t pid;    /* value, returned by fork(2)                     */
    int in;       /* Zero file descriptor                           */
    int out;      /* First file descriptor                          */
    bool_t piped; /* True if process connected by pipe to the other */
    bool_t bg;    /* Must be run in the background                  */

    struct termios term; /* Terminal state of stopped process */ 
};

#endif /* _COMMAND_H_ */
