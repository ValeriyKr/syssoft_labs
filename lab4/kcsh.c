/* Made by ValeriyKr (s207214) */
/*
 * vim: sw=8 :
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"


#define ERROR_MSG(x) "\nkcsh: " x

#define CMD_LENGTH 1024


enum error_in {
        E_WRITE,
        E_READ,
        E_FORK,
        E_EXEC
};


/*
 * Says something to stdout
 *
 * \param message message to say
 */
void say(const char *message);

/*
 * Prints error to stderr and exits if exit is required
 *
 * \param subj subkect of error
 * \param do_exit ono-zero if want to exit after this error
 */
void error(enum error_in subj, int do_exit);

/* Does fork end executes subprocess.
 * Executes error(E_WRITE, 1) if is shutted up.
 *
 * \param args arguments which will be passed to process
 * \return exit status of executed command. 0 is usually good.
 */
int fork_and_exec(char **args);



void say(const char *message) {
        if (NULL == message) return;
        if (-1 == write(1, message, strlen(message))) {
                error(E_WRITE, 1);
        }
}


void error(enum error_in subj, int do_exit) {
        int exit_code;
        switch (subj) {
                case E_WRITE:
                {
                        const char err[] = ERROR_MSG("cannot write to stdout");
                        write(2, err, sizeof(err));
                        exit_code = 1;
                        break;
                }
                case E_READ:
                {
                        const char err[] = ERROR_MSG("cannot read stdin");
                        write(2, err, sizeof(err));
                        exit_code = 2;
                        break;
                }
                case E_FORK:
                {
                        const char err[] = ERROR_MSG("cannot do fork");
                        write(2, err, sizeof(err));
                        exit_code = 3;
                        break;
                }
                case E_EXEC:
                {
                        const char err[] = ERROR_MSG("cannot do exec");
                        write(2, err, sizeof(err));
                        exit_code = 4;
                        break;
                }
                default:
                {
                        const char *err = ERROR_MSG("unknown error");
                        write(2, err, sizeof(err));
                        exit_code = 255;
                }
        }
        if (do_exit) exit(exit_code);
}


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

        /* PS1 in your sweet Bourne-compatible shells */
        setenv("prompt", "% ", 1);
        while (1) {
                ssize_t cmd_len;
                char **args;
                say(getenv("prompt"));
                cmd_buff[0] = '\0';
                if (-1 == (cmd_len = read(0, cmd_buff, CMD_LENGTH - 1))) {
                        error(E_READ, 2);
                }
                if (0 == cmd_len) {
                        exit(0); /* EOF occurred */
                }
                cmd_buff[cmd_len] = '\0';
                args = parse_cmd(cmd_buff);
                fork_and_exec(args);
        }
}
