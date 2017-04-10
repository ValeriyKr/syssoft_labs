/* Made by ValeriyKr (s207214) */
/*
 * vim: sw=8 :
 */

#include <stdlib.h>
#include <string.h>
#include "parser.h"

/*
 * Makes new argument and puts it into collection.
 *
 * \param line line with command
 * \param from start position of argument
 * \param to end position of argument
 * \param args array of arguments
 */
static void
make_arg(const char *line, size_t from, size_t to, char **args, size_t count);


char** parse_cmd(const char *line) {
        size_t count = 0;
        size_t line_len = strlen(line);
        size_t i;
        size_t prev = 0;

        if (NULL == line) return NULL;
        /* Parse arguments. Parse strings. Parse everything. Even your life. */
        for (i = 0; i < line_len; ++i) {
                if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
                        make_arg(line, prev, i, NULL);
                }
        }
}


static void
make_arg(const char *line, size_t from, size_t to, char **args, size_t count) {
        char *arg = (char*) malloc(to - from + 2);
        if (NULL == arg) return;
        strncpy(arg, line, to - from + 1);
        /* TODO: add `arg` to `args` */
}
