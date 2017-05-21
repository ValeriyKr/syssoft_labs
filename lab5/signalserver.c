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
        signal(signo, sighandler);
        while (!update());
        switch (signo) {
        case SIGHUP:
                printf("PID: %d\n", state.pid);
                break;
        case SIGINT:
                printf("UID: %u\n", state.uid);
                break;
        case SIGTERM:
                printf("GID: %u\n", state.gid);
                break;
        case SIGUSR1:
                printf("Working time: %lu\n", state.working_time);
                break;
        case SIGUSR2:
                printf("Load average for 1 minute: %lf\n", state.loadavg[0]);
                printf("Load average for 5 minutes: %lf\n", state.loadavg[1]);
                printf("Load average for 15 minutes: %lf\n", state.loadavg[2]);
        }
}


int main() {
        int sleep_ret;
        struct sigaction act;

        if (-1 == time(&state.start_time)) {
                perror("time");
                _exit(1);
        }
        state.pid = getpid(); /* "These functions are always successful." */
        state.uid = getuid();
        state.gid = getgid();

        memset(&act, 0, sizeof(act));
        act.sa_handler = sighandler;
        sigfillset(&act.sa_mask);

        sigaction(SIGHUP, &act, NULL);
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
        sigaction(SIGUSR1, &act, NULL);
        sigaction(SIGUSR2, &act, NULL);

        while (sleep_ret = usleep(1000000), true) {
                if (-1 == sleep_ret) {
                        if (EINTR == errno) goto force_update;
                        perror("usleep");
                        _exit(3);
                }
force_update:
                (void) update();
        }
}
