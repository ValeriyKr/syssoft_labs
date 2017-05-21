#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "state.h"


static struct state state;
static int fd = -1;


static bool_t update() {
        time_t current_time;
        if (-1 == time(&current_time)) {
                perror("time");
                _exit(1);
        }
        if (current_time - state.start_time > state.working_time) {
                state.working_time = current_time - state.start_time;
                if (-1 == getloadavg(state.loadavg, 3)) {
                        perror("loadavg");
                        _exit(2);
                }
                return true;
        }
        return false;
}


static void sighandler(int signo) {
        if (signo == SIGUSR1) {
                signal(signo, sighandler);
                printf("Working time: %lu\nPID: %d\nUID: %u\nGID: %u\n",
                                state.working_time, state.pid, state.uid,
                                state.gid);
                printf("Load average for 1 minute: %lf\n", state.loadavg[0]);
                printf("Load average for 5 minutes: %lf\n", state.loadavg[1]);
                printf("Load average for 15 minutes: %lf\n", state.loadavg[2]);
        } else {
                close(fd);
                unlink(SOCK_FNAME);
                _exit(0);
        }
}


int main(int c, char *v[]) {
        int cfd, unlink_ret;
        struct sockaddr_un addr;
        size_t i;

        if (-1 == time(&state.start_time)) {
                perror("time");
                _exit(1);
        }
        state.pid = getpid(); /* "These functions are always successful." */
        state.uid = getuid();
        state.gid = getgid();

        for (i = 1; i < 30; signal(i++, sighandler));
        
        if (-1 == (fd = socket(AF_UNIX, SOCK_STREAM, 0))) {
                perror("socket");
                _exit(2);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCK_FNAME, sizeof(addr.sun_path) - 1);
        if (-1 == (unlink_ret = unlink(SOCK_FNAME)) && errno != ENOENT) {
                perror("unlink");
                printf("%d\n", unlink_ret);
                _exit(2);
        }
        if (-1 == bind(fd, (struct sockaddr *) &addr,
                       sizeof(struct sockaddr_un))) {
                perror("bind");
                _exit(2);
        }
        if (-1 == listen(fd, 50)) {
                perror("listen");
                _exit(2);
        }
        while (true) {
                if (-1 == (cfd = accept(fd, NULL, NULL))) {
                        perror("accept");
                        _exit(3);
                }
                while (!update());
                if (sizeof(state) != write(cfd, &state, sizeof(state))) {
                        perror("write");
                        _exit(4);
                }
                close(cfd);
        }
}
