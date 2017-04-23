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

#endif /* _BUILTINS_IMPL_H_ */
