/* Made by ValeriyKr (s207214) */
/*
 * vim: sw=8 :
 */

#ifndef _PARSER_H_
#define _PARSER_H_

/*
 * Parses line to argv-like array.
 *
 * \param line string to parse
 * \return array of arguments. Last is NULL
 */
char **parse_cmd(const char *line);


#endif /* _PARSER_H_ */
