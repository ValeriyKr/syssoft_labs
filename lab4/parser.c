#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "globdef.h"
#include "io.h"


static bool_t is_space(char c) {
        return (c == ' ')
                || (c == '\t');
}


static bool_t is_space_or_eol(char c) {
        return is_space(c) || (c == '\n');
}


static void skip_spaces(const char **line) {
        for (; is_space(**line); (*line)++);
}


static char* get_arg(const char *line) {
        size_t p = 0;
        char *arg;
        for (; line[p] && !is_space(line[p]); ++p);
        assert(NULL != (arg = (char*) malloc(p)));
        strncpy(arg, line, p);
        arg[p] = '\0';
        return arg;
}


static void put(char **vec, const char *item) {
        size_t i;
        assert(vec);
        assert(item);

        for (i = 0; NULL != vec[i]; ++i);
        if (i + 1 > ARGS_SIZE) {
                sayi(i);
                say("\n");
                error(E_MANYARGS, 1);
        }
        assert(NULL != (vec[i] = (char *) malloc(strlen(item))));
        strcpy(vec[i], item);
        vec[i+1] = NULL;
}


char** get_splitted(const char *line) {
        size_t pos = 0;
        char **args;

        assert(NULL != (args = (char **) malloc(ARGS_SIZE * sizeof(char *))));
        args[0] = NULL;
        while (line[pos]) {
                char *arg;
                skip_spaces(&line);
                if (!line[0]) break;
                arg = get_arg(line);
                line += strlen(arg);
                put(args, arg);
        }
        return args;
}
