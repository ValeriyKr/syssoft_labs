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
#include "builtin.h"
#include "builtins_impl.h"
#include "variable.h"
#include "job.h"

/**
 * \file kcsh.c
 * \author kk
 *
 * \brief csh-like shell
 *
 * Implementation of shell, which can be used as C shell.
 */


/**
 * Does fork end executes subprocess. Also checks if command is ahell built-in.
 *
 * Executes error(E_WRITE, 1) if is shutted up.
 *
 * \param command command for execution.
 * \return exit status of executed command. 0 is usually good.
 */
int fork_and_exec(struct cmd *command);


static int pipefd[2];


int fork_and_exec(struct cmd *command) {
        pid_t pid;
        int st;
        char **args = command->argv;

        if (args == NULL) return 1;
        if (command->argc == 0) return 0;
        pid = fork();
        if (0 > pid) {
                error(E_FORK, 0);
                return 13;
        } else if (0 == pid) {
                builtin_func_t bt = builtin(command->argv[0]);
                if (command->in != 0) {
                        if (-1 == dup2(command->in, 0)) {
                                error(E_DUP, 0);
                                return 15;
                        }
                }
                if (command->out != 1) {
                        if (-1 == dup2(command->out, 1)) {
                                error(E_DUP, 1);
                                return 15;
                        }
                }
                if (command->in != 0) close(pipefd[0]);
                if (command->out != 1) close(pipefd[1]);
                if (command->bg) setpgid(0, 0);
                if (NULL != bt) {
                        return bt(command->argc, command->argv);
                }
                if (-1 == execvp(args[0], args)) {
                        error(E_EXEC, 0);
                        return 14;
                }
        } else {
                if (command->bg) {
                        sayc('[');
                        sayi(add_job(pid));
                        say("] ");
                        sayi(pid);
                        sayln("");
                        return 0;
                }
                while (1) {
                        waitpid(pid, &st, WUNTRACED);
                        if (WIFEXITED(st)) {
                                return WEXITSTATUS(st);
                        }
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
pid_t kcsh_pid;


int main(int argc, char *argv[]) {
        char *line;
        struct cmd **commands;

        kcsh_argc = argc;
        kcsh_argv = argv;
        kcsh_pid = getpid();
        goodmorning();
        TRY_ALLOC(commands = malloc(sizeof(struct cmd) * 1024));
        while (1) {
                size_t i;
                char **args;
                int err_code = 0;
                char *err = NULL;
                int st;
                pid_t dead_child_pid;

                if (0 < (dead_child_pid = waitpid(-1, &st, WNOHANG))) {
                        sayc('[');
                        sayi(get_job_id(dead_child_pid));
                        say("] (pid ");
                        sayi(dead_child_pid);
                        say(") ");
                        say("done");
                        delete_job_by_pid(dead_child_pid);
                        sayln("");
                }
                init_term();
                line = get_line();
                reset_term();
                args = get_splitted(line);
                commands[0] = NULL;
                if (!make_cmdv(args, commands)) {
                        error(E_SYNTAX, 0);
                        for (i = 0; commands[i]; ++i) {
                                free_cmd(commands[i]);
                        }
                        goto clean;
                }
                for (i = 0; commands[i]; ++i) {
                        bool_t did_pipe = false;
                        if (commands[i]->out != 1) {
                                if (-1 == pipe(pipefd)) {
                                        error(E_PIPE, 1);
                                        return 16;
                                }
                                did_pipe = true;
                                commands[i]->out = pipefd[1];
                                if (commands[i+1])
                                        commands[i+1]->in = pipefd[0];
                        }
                        err_code = fork_and_exec(commands[i]);
                        if (did_pipe)
                                close(pipefd[1]);
                        free_cmd(commands[i]);
                }
                err = errc_to_a(err_code);
                set_var_force("?", err);
                set_var_force("_", line);
clean:
                free(err);
                free(line);
                free(args);
        }
        /* free(commands); */
        /* goodnight(0);   */
}


void signal_handler(int signal) {
        switch (signal) {
        case SIGINT:
                /* TODO: check all traps */
                say("^C");
                break;
        case SIGCHLD:
        {
                int st;
                pid_t pid = waitpid(-1, &st, 0);
                say("Done: ");
                sayi(pid);
                sayc('\n');
                break;
        }
        case SIGTERM:
                goodnight(0);
        default:
                return;
        }
}


void goodmorning() {
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);
        /*signal(SIGCHLD, signal_handler);*/
        init_term();
        setenv("shell", kcsh_argv[0], 1);
        /* PS1 in your sweet Bourne-compatible shells */
        setenv("prompt", "% ", 1);
        init_builtins();

        set_var_force("?", "0");
        set_var_force("_", "");

        if (register_builtin("?", k_help)
                || register_builtin("cd", k_cd)
                || register_builtin("set", k_set)
                || register_builtin("setenv", k_setenv)
                || register_builtin("exit", k_exit)) {
                error(E_BREG, 1);
        }
}


void goodnight(int exit_code) {
        int st;
        waitpid(-1, &st, 0);
        reset_term();
        _exit(exit_code);
}
