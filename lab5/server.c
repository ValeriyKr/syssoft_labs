#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "state.h"


static struct state state;
static int shmid = -1;
static struct state *shmaddr;


static bool_t update() {
        time_t current_time;
        if (-1 == time(&current_time)) {
                perror("time");
                shmdt(shmaddr);
                _exit(1);
        }
        if (current_time - state.start_time > state.working_time) {
                state.working_time = current_time - state.start_time;
                if (-1 == getloadavg(state.loadavg, 3)) {
                        perror("loadavg");
                        shmdt(shmaddr);
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
                shmdt(shmaddr);
                _exit(0);
        }
}


int main() {
        int sleep_ret;
        int i;
        for (i = 1; i < 30; ++i, signal(i, sighandler));
        if (-1 == time(&state.start_time)) {
                perror("time");
                _exit(1);
        }
        state.pid = getpid(); /* "These functions are always successful." */
        state.uid = getuid();
        state.gid = getgid();

        if (-1 == (shmid = shmget(2987, sizeof(struct state),
                                        IPC_CREAT | 0666))) {
                perror("shmget");
                _exit(3);
        }
        if (-1 == (long) (shmaddr = (struct state*) shmat(shmid, NULL, 0))) {
                perror("shmat");
                _exit(4);
        }

        while (sleep_ret = usleep(1000), true) {
                if (-1 == sleep_ret) {
                        if (EINTR == errno) goto force_update;
                        perror("usleep");
                        shmdt(shmaddr);
                        _exit(5);
                }
force_update:
                if (update()) {
                        memcpy(shmaddr, &state, sizeof(struct state));
                }
        }
}
