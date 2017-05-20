#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#include "state.h"


static int msg_main(void);


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
                        /* mmap_main(); */
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
                shmctl(shmid, IPC_RMID, NULL);
                _exit(4);
        }

        printf("Working time: %lu\nSrv pid: %d\nSrv uid: %u\nSrv gid: %u\n",
                        shmaddr->working_time, shmaddr->pid, shmaddr->uid,
                        shmaddr->gid);
        printf("Load average for 1 minute: %lf\n", shmaddr->loadavg[0]);
        printf("Load average for 5 minutes: %lf\n", shmaddr->loadavg[1]);
        printf("Load average for 15 minutes: %lf\n", shmaddr->loadavg[2]);

        shmdt(shmaddr);
        shmctl(shmid, IPC_RMID, NULL);

        return 0;
}


int msg_main() {
        int msgid = -1;
        struct msg_buf msg_buf;
        struct state *state;

        if (-1 == (msgid = msgget(2987, IPC_CREAT | 0666))) {
                perror("msgget");
                _exit(3);
        }

        if (-1 == msgrcv(msgid, &msg_buf, sizeof(struct state), 1, 0)) {
                perror("msgrcv");
                msgctl(msgid, IPC_RMID, NULL);
                _exit(4);
        }
        state = (struct state *) &(msg_buf.mtext);

        printf("Working time: %lu\nSrv pid: %d\nSrv uid: %u\nSrv gid: %u\n",
                        state->working_time, state->pid, state->uid,
                        state->gid);
        printf("Load average for 1 minute: %lf\n", state->loadavg[0]);
        printf("Load average for 5 minutes: %lf\n", state->loadavg[1]);
        printf("Load average for 15 minutes: %lf\n", state->loadavg[2]);

        msgctl(msgid, IPC_RMID, NULL);

        return 0;
}
