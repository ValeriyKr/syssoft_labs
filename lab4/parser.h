#ifndef _PARSER_H_
#define _PARSER_H_

#include "globdef.h"
#include "command.h"

#define ARGS_SIZE 1024


/**
 * \param line null-terminated string.
 * \return vector of strings, separated by space.
 */
char** get_splitted(char *line);


/**
 * Fills vector of commands with commands, made from splitted input.
 *
 * \param args vector of tokens. Destroys elements after using 'em.
 * \param commands array to fill.
 * \return true if success.
 */
bool_t make_cmdv(char **args, struct cmd **commands);


/**
 * Destroys entity of `struct cmd` type.
 *
 * \param c pointer to command.
 */
void free_cmd(struct cmd *c);


/**
 * Checks that symbol is space character.
 *
 * \param c character for checking.
 * \return true if `c` is space.
 */
bool_t is_space(char c);

/**
 * Checks that symbol is space character or end of line.
 *
 * \param c character for checking.
 * \return true if `c` is space or EOL.
 */
bool_t is_space_or_eol(char c);


#endif /* _PARSER_H_ */
