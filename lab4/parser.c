#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "command.h"
#include "globdef.h"
#include "io.h"
#include "variable.h"


bool_t is_space(char c) {
        return (c == ' ')
                || (c == '\t');
}


bool_t is_space_or_eol(char c) {
        return is_space(c) || (c == '\n');
}


static void skip_spaces(const char **line) {
        for (; is_space(**line); (*line)++);
}


static bool_t is_separator(char c) {
        return (c == ';');
}


static char* get_arg(const char *line) {
        size_t p = 0;
        char *arg;
        for (; line[p] && !is_space(line[p]); ++p);
        TRY_ALLOC(arg = (char*) malloc(p));
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
                sayul(i);
                say("\n");
                error(E_MANYARGS, 1);
        }
        TRY_ALLOC(vec[i] = (char *) malloc(strlen(item) + 1));
        strcpy(vec[i], item);
        vec[i][strlen(item)] = '\0';
        vec[i+1] = NULL;
}


static void unfold_variable(char *line) {
        const struct variable * vars; 
        size_t vars_c, i;
        char *subs, *env;

        if (line[0] != '$') return;
        vars = get_vars();
        vars_c = get_vars_count();
        for (i = 0; i < vars_c; ++i) {
                size_t n_len = strlen(vars[i].name);
                if (!strncmp(vars[i].name, line+1, n_len)) {
                        size_t l_len = strlen(line);
                        size_t v_len = strlen(vars[i].value);
                        char *buf;
                        TRY_ALLOC(buf = (char *) malloc(l_len + v_len + 1));
                        strcpy(buf, vars[i].value);
                        strcat(buf, line + n_len + 1);
                        strcpy(line, buf);
                        free(buf);
                        return;
                }
        }
        TRY_ALLOC(subs = (char *) malloc(strlen(line) + 1));
        for (i = 1; line[i]; ++i) {
                strncpy(subs, line + 1, i);
                subs[i+1] = '\0';
                if (NULL != (env = getenv(subs))) {
                        size_t v_len = strlen(env);
                        size_t l_len = strlen(line);
                        char *buf;
                        TRY_ALLOC(buf = (char *) malloc(l_len + v_len + 1));
                        strcpy(buf, env);
                        strcat(buf, line + i + 1);
                        strcpy(line, buf);
                        free(buf);
                        goto unfold_variable_clean;
                }
        }
unfold_variable_clean:
        free(subs);
        return;
}


char** get_splitted(char *line) {
        size_t pos;
        char **args;

        TRY_ALLOC(args = (char **) malloc(ARGS_SIZE * sizeof(char *)));
        args[0] = NULL;
        for (pos = 0; line[pos]; ++pos) {
                unfold_variable(line + pos);
        }
        pos = 0;
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


static struct cmd * make_cmd(char **args, size_t first, size_t last) {
        size_t i;
        struct cmd *c;

        assert(last >= first);
        TRY_ALLOC(c = (struct cmd *) malloc(sizeof(struct cmd)));
        TRY_ALLOC(c->argv = (char **) malloc(sizeof(char*)
                                * (last - first + 2)));

        /* c->argc  = last - first + 1; */
        c->argc  = 0;
        c->in    = 0;
        c->out   = 0;
        c->piped = false;
        c->bg    = false;
        for (i = first; i <= last; ++i) {
                c->argv[c->argc++] = args[i];
                args[i] = NULL;
        }
        c->argv[c->argc] = NULL;
        return c;
}


bool_t make_cmdv(char **args, struct cmd **commands) {
        size_t first, last, length, i;

        assert(NULL != commands);

        first = last = length = 0;
        for (i = 0; args[i]; ++i) {
                if (is_separator(args[i][0]) || !args[i+1]) {
                        last = i - 1;
                        if (!is_separator(args[i][0]))
                                last = i;
                        commands[length++] = make_cmd(args, first, last);
                        commands[length] = NULL;
                        for (; args[i] && is_separator(args[i][0]); ++i);
                        first = last = i;
                } else {
                        ++last;
                }
        }
        return true;
}


void free_cmd(struct cmd *c) {
        size_t i;
        for (i = 0; c->argv[i]; ++i) {
                free(c->argv[i]);
        }
        free(c);
}
