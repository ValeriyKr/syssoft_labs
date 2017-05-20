#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "state.h"


static struct state state;
static int shmid = -1;
static struct state *shmaddr;


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


int main() {
        int sleep_ret;
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
                        perror("usleep");
                        _exit(5);
                }
                if (update()) {
                        memcpy(shmaddr, &state, sizeof(struct state));
                }
        }
}
