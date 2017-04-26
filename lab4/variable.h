#ifndef _VARIABLE_H_
#define _VARIABLE_H_

/**
 * \file variable.h
 * \author kk
 *
 * Contains definition of `variable` structure and methods for working with
 * it.
 */

/**
 * \struct variable
 *
 * Describes internal shell variable.
 */
struct variable {
        char *name;  /**< Variable's name  */
        char *value; /**< Variable's value */
};


/**
 * Returns variable value by name.
 * Finds it in internal variables, then in environment.
 *
 * \param name variable name.
 * \return value of variable or NULL if there is no match.
 */
const char * get_var(const char *name);


/**
 * \return array of all internal variables.
 */
const struct variable * get_vars(void);


/**
 * \return count of internal variables.
 */
size_t get_vars_count(void);


/**
 * Assigns value to variable.
 *
 * \param name name of variable.
 * \param value new value of variable.
 */
void set_var(const char *name, const char *value);

#endif /* _VARIABLE_H_ */
