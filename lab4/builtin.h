#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include "globdef.h"

/**
 * \file builtin.h
 * \author kk
 * \brief Definition of built-in servicing functions.
 *
 * This files (builtin.h and builtin.c) describes system of kcsh's built-in
 * functions (built-ins).
 *
 * Structure `struct builtin` describes one built-in. It has `name` and 
 * `command` fields. When user types a command, the first arguments shall be
 * checked for belonging to built-ins' collection by invocation of `builtin`
 * function. It will return pointer to function, which must be executed or NULL.
 *
 * Collection must be initialized before the work by calling `init_builtins`
 * function. After this, you can replenish it with your built-ins by calling
 * `register_builtin` function.
 *
 * I don't think i have to implement methods for deallocation of collection,
 * 'cos they will be invoked only in the end, when programme finishes and
 * deallocations happen automatically.
 *
 * TODO: to write the new system of built-in functions, with using dynamic
 * libraries, auto-expansion of built-ins' collection and dealing without manual
 * registration of them.
 */

/**
 * \typedef builtin_func_t
 * \brief Type of built-in function
 *
 * Built-in functions shall receive count of arguments and vector of the
 * arguments. The first argument must be name, which user typed.
 *
 * Built-ins return error code, as it does usual programme.
 */
typedef int (*builtin_func_t)(size_t, char **);

/**
 * \struct builtin
 * Describes one shell built-in function.
 */
struct builtin {
        char *name;             /**< Name of built-in function */
        builtin_func_t command; /**< Command to execute        */
};


/**
 * Initializes collection of built-ins.
 */
void init_builtins(void);

/**
 * Registers built-in in collection.
 *
 * \param name name of builtin, users types it when he wants to call built-in.
 * \param f function which will be invoked, when user call built-in.
 * \return 0 if built-in was successfully added.
 */
int register_builtin(const char *name, builtin_func_t f);

/**
 * Returns built-in's function by built-in's name.
 *
 * \param name name of built-in in collection.
 * \return function if successfully found, NULL otherwise.
 */
builtin_func_t builtin(const char *name);

#endif /* _BUILTIN_H_ */
