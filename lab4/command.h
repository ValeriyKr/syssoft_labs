#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <termios.h>
#include <sys/types.h>

#include "globdef.h"

/**
 * \file command.h
 * \brief File contains definition of executable command structure.
 *
 * This file contains definition of `struct cmd`, structure which describes
 * context of executable command.
 */

/** \struct cmd
 * Describes context of executable command.
 */
struct cmd {
        int argc;     /**< Arguments count                                */
        char **argv;  /**< Arguments                                      */
        pid_t pid;    /**< value, returned by fork(2)                     */
        int in;       /**< Zero file descriptor                           */
        int out;      /**< First file descriptor                          */
        bool_t piped; /**< True if process connected by pipe to the other */
        bool_t bg;    /**< Must be run in the background                  */
        bool_t redirect;     /**< True if output is redirected to file    */
        bool_t rappend;      /**< True if command shall append file       */
        char *rfilename;     /**< Output redirection filename             */
        bool_t redirect_in;  /**< True if input is redirected from file   */
        char *rifilename;    /**< Input redirection filename              */

        struct termios term; /**< Terminal state of stopped process */ 
};

#endif /* _COMMAND_H_ */
