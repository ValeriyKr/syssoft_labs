#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "state.h"


int msg_main(void);
int mmap_main(void);


int main(int c, char *v[]) {
        int shmid;
        struct state *shmaddr;

        switch (c) {
        case 1:
                goto shmem_main;
        case 2:
                if (!strcmp(v[1], "msg")) {
                        return msg_main();
                } else if (!strcmp(v[1], "mmap")) {
                        return mmap_main();
                } else if (!strcmp(v[1], "shm")) {
                        goto shmem_main;
                }
        default:
                fprintf(stderr, "USAGE: %s [shm|msg|mmap]\n", v[0]);
                _exit(0);
        }

shmem_main:
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


int msg_main() {
        int msgid = -1;
        struct msg_buf msg_buf;
        struct state *state;

        if (-1 == (msgid = msgget(2987, 0666))) {
                perror("msgget");
                _exit(3);
        }

        if (-1 == msgrcv(msgid, &msg_buf, sizeof(struct state), 1,
                         IPC_NOWAIT)) {
                perror("msgrcv");
                _exit(4);
        }
        state = (struct state *) &(msg_buf.mtext);

        printf("Working time: %lu\nSrv pid: %d\nSrv uid: %u\nSrv gid: %u\n",
                        state->working_time, state->pid, state->uid,
                        state->gid);
        printf("Load average for 1 minute: %lf\n", state->loadavg[0]);
        printf("Load average for 5 minutes: %lf\n", state->loadavg[1]);
        printf("Load average for 15 minutes: %lf\n", state->loadavg[2]);

        return 0;
}


int mmap_main() {
        int fd;
        struct state *state;
        if (-1 == (fd = shm_open(S_FNAME, O_RDWR, 0))) {
                perror("shm_open");
                _exit(3);
        }

        if (MAP_FAILED == (state = mmap(NULL, sizeof(struct state), PROT_READ,
                                        MAP_SHARED, fd, 0))) {
                perror("mmap");
                _exit(4);
        }

        printf("Working time: %lu\nSrv pid: %d\nSrv uid: %u\nSrv gid: %u\n",
                        state->working_time, state->pid, state->uid,
                        state->gid);
        printf("Load average for 1 minute: %lf\n", state->loadavg[0]);
        printf("Load average for 5 minutes: %lf\n", state->loadavg[1]);
        printf("Load average for 15 minutes: %lf\n", state->loadavg[2]);

        if (-1 == munmap(state, sizeof(struct state))) {
                perror("munmap");
                _exit(5);
        }

        return 0;
}
