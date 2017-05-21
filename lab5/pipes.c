#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


int main(int c, char *v[]) {
        pid_t pid;
        int pipefd[2], fd;

        if (c != 2) {
                fprintf(stderr, "USAGE: %s <filename>", v[0]);
                _exit(1);
        }

        if (-1 == (fd = open(v[1], O_RDONLY, 0))) {
                perror("open");
                _exit(2);
        }

        if (-1 == pipe(pipefd)) {
                perror("pipe");
                _exit(3);
        }

        if (-1 == (pid = fork())) {
                perror("fork");
                _exit(4);
        } else if (0 == pid) {
                close(pipefd[1]);
                close(STDIN_FILENO);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                execlp("wc", "wc", (char *) NULL);
                perror("execlp");
                _exit(5);
                /* Emit SIGCHLD on exit, parent will die too, it's ok. */
        } else {
                char buf[2];
                close(pipefd[0]);
                while (2 == read(fd, buf, 2)) {
                        write(pipefd[1], buf + 1, 1);
                }
                close(pipefd[1]);
                close(fd);
        }

        return 0;
}
