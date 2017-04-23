#ifndef _BUILTINS_IMPL_H_
#define _BUILTINS_IMPL_H_


/**
 * cd [NEW_DIRECTORY]
 *
 * Changes working directory to NEW_DIRECTORY or to $home if not specified.
 */
int k_cd(size_t argc, char **argv);

/**
 * exit [CODE]
 *
 * Stop working with exit [CODE] or 0 if not specified.
 */
int k_exit(size_t argc, char **argv);

/**
 * setenv [NAME [VALUE]]
 *
 * Without arguments, prints all environment variables. If NAME was specified,
 * sets NAME to VALUE if it was specified, otherwise to NULL.
 */
int k_setenv(size_t argc, char **argv);

/**
 * set NAME=[VALUE]
 *
 * Assigns VALUE (or empty string) to variable NAME
 */
int k_set(size_t argc, char **argv);

/**
 * ?
 *
 * Shows help.
 */
int k_help(size_t argc, char **argv);

#endif /* _BUILTINS_IMPL_H_ */
