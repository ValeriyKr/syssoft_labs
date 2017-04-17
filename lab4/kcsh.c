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

#include "globdef.h"
#include "parser.h"
#include "io.h"


#define CMD_LENGTH 1024


/* Does fork end executes subprocess.
 * Executes error(E_WRITE, 1) if is shutted up.
 *
 * \param args arguments which will be passed to process
 * \return exit status of executed command. 0 is usually good.
 */
int fork_and_exec(char **args);


int fork_and_exec(char **args) {
        pid_t pid, wpid;
        int st;
        if (args == 0) return 1;
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


int main(int argc, char *argv[]) {
        char cmd_buff[CMD_LENGTH];

        goodmorning();
        while (1) {
                size_t it;
                ssize_t cmd_len;
                char **args;
                char *line = get_line();
                sayln(line);
                args = get_splitted(line);
                for (it = 0; args[it]; ++it) {
                        sayln(args[it]);
                }
                fork_and_exec(args);
                free(args);
                free(line);
        }
        goodnight(0);
}


void sigint_handler(int signal) {
        /* TODO: check all traps */
        say("^C");
}


void goodmorning() {
        init_term();
        /* PS1 in your sweet Bourne-compatible shells */
        setenv("prompt", "% ", 1);
        signal(SIGINT, sigint_handler);
}


void goodnight(int exit_code) {
        reset_term();
        exit(exit_code);
}
