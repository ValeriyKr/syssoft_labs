/* Made by ValeriyKr (s207214) */
/*
 * vim: sw=8 :
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

#include "globdef.h"
#include "parser.h"
#include "io.h"
#include "command.h"



/* Does fork end executes subprocess.
 * Executes error(E_WRITE, 1) if is shutted up.
 *
 * \param args arguments which will be passed to process
 * \return exit status of executed command. 0 is usually good.
 * TODO: upd descr
 */
int fork_and_exec(struct cmd *command);


int fork_and_exec(struct cmd *command) {
        pid_t pid, wpid;
        int st;
        char **args = command->argv;

        if (args == NULL) return 1;
        if (command->argc == 0) return 0;
        pid = fork();
        if (0 > pid) {
                error(E_FORK, 0);
                return 13;
        } else if (0 == pid) {
               if (-1 == execvp(args[0], args)) {
                       error(E_EXEC, 0);
                       return 14;
               }
        } else {
                while (1) {
                        wpid = waitpid(pid, &st, WUNTRACED);
                        if (WIFEXITED(st))
                                return WEXITSTATUS(st);
                        if (WIFSIGNALED(st)) {
                                say("Killed");
                                return 0;
                        }
                }
        }
        /* Make clang kinder */
        return 0;
}


int kcsh_argc;
char **kcsh_argv;


int main(int argc, char *argv[]) {
        char *line;
        struct cmd **commands;

        kcsh_argc = argc;
        kcsh_argv = argv;
        goodmorning();
        assert(NULL != (commands = malloc(sizeof(struct cmd) * 1024)));
        while (1) {
                size_t i;
                char **args;

                init_term();
                line = get_line();
                reset_term();
                args = get_splitted(line);
                commands[0] = NULL;
                if (!make_cmdv(args, commands)) {
                        error(E_SYNTAX, 0);
                        goto clean;
                }
                for (i = 0; commands[i]; ++i) {
                        sayi(fork_and_exec(commands[i]));
                        free_cmd(commands[i]);
                }
clean:
                free(line);
                free(args);
        }
        /* free(commands); */
        /* goodnight(0);   */
}


void sigint_handler(int signal) {
        /* TODO: check all traps */
        say("^C");
}


void goodmorning() {
        init_term();
        setenv("shell", kcsh_argv[0], 1);
        /* PS1 in your sweet Bourne-compatible shells */
        setenv("prompt", "% ", 1);
        signal(SIGINT, sigint_handler);
}


void goodnight(int exit_code) {
        reset_term();
        exit(exit_code);
}
