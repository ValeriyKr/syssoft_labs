#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include "state.h"


int main() {
        int shmid;
        struct state *shmaddr;

        if (-1 == (shmid = shmget(2987, sizeof(struct state), 0666))) {
                perror("shmget");
                _exit(3);
        }

        if (-1 == (long) (shmaddr = (struct state*) shmat(shmid, NULL, 0))) {
                perror("shmat");
                _exit(4);
        }

        printf("Working time: %lu\nSrv pid: %d\nSrv uid: %u\nSrv gid: %u\n",
                        shmaddr->working_time, shmaddr->pid, shmaddr->uid,
                        shmaddr->gid);
        printf("Load average for 1 minute: %lf\n", shmaddr->loadavg[0]);
        printf("Load average for 5 minutes: %lf\n", shmaddr->loadavg[1]);
        printf("Load average for 15 minutes: %lf\n", shmaddr->loadavg[2]);

        shmdt(shmaddr);

        return 0;
}
