#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "state.h"


static struct state state;
static int shmid = -1;
static int msgid = -1;
static struct state *shmaddr;
static int fd = -1;
static struct state *mapaddr;


static bool_t update() {
        time_t current_time;
        if (-1 == time(&current_time)) {
                perror("time");
                shmdt(shmaddr);
                shmctl(shmid, IPC_RMID, NULL);
                msgctl(msgid, IPC_RMID, NULL);
                _exit(1);
        }
        if (current_time - state.start_time > state.working_time) {
                state.working_time = current_time - state.start_time;
                if (-1 == getloadavg(state.loadavg, 3)) {
                        perror("loadavg");
                        shmdt(shmaddr);
                        shmctl(shmid, IPC_RMID, NULL);
                        msgctl(msgid, IPC_RMID, NULL);
                        munmap(mapaddr, sizeof(struct state));
                        shm_unlink(S_FNAME);
                        _exit(2);
                }
                return true;
        }
        return false;
}


static void shm_sighandler(int signo) {
        if (signo == SIGUSR1) {
                signal(signo, shm_sighandler);
                printf("Working time: %lu\nPID: %d\nUID: %u\nGID: %u\n",
                                state.working_time, state.pid, state.uid,
                                state.gid);
                printf("Load average for 1 minute: %lf\n", state.loadavg[0]);
                printf("Load average for 5 minutes: %lf\n", state.loadavg[1]);
                printf("Load average for 15 minutes: %lf\n", state.loadavg[2]);
        } else {
                shmdt(shmaddr);
                shmctl(shmid, IPC_RMID, NULL);
                _exit(0);
        }
}


int msg_main(void);
int mmap_main(void);


int main(int c, char *v[]) {
        int sleep_ret;
        int i;
        char *bname;
        if (-1 == time(&state.start_time)) {
                perror("time");
                _exit(1);
        }
        state.pid = getpid(); /* "These functions are always successful." */
        state.uid = getuid();
        state.gid = getgid();

        for (bname = v[0], i = 0; v[0][i]; ++i) {
                if ('/' == v[0][i])
                        bname = v[0] + i;
        }
        if (*bname == '/') bname++;
        switch (c) {
        case 1:
                if (!strcmp("shmserver", bname)) {
                        puts("1234\n");
                        goto shmem_main;
                } else if (!strcmp("msgserver", bname)) {
                        return msg_main();
                } else if (!strcmp("mmapserver", bname)) {
                        return mmap_main();
                }
        case 2:
                if (!strcmp(bname, "server")) {
                        if (!strcmp(v[1], "msg")) {
                                return msg_main();
                        } else if (!strcmp(v[1], "mmap")) {
                                return mmap_main();
                        } else if (!strcmp(v[1], "shm")) {
                                goto shmem_main;
                        }
                }
        default:
                fprintf(stderr, "USAGE: %s [shm|msg|mmap]\n", v[0]);
                _exit(0);
        }

shmem_main:
        for (i = 1; i < 30; signal(i++, shm_sighandler));
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
                        shmctl(shmid, IPC_RMID, NULL);
                        _exit(5);
                }
force_update:
                if (update()) {
                        memcpy(shmaddr, &state, sizeof(struct state));
                }
        }
}


static void msg_sighandler(int signo) {
        if (signo == SIGUSR1) {
                signal(signo, shm_sighandler);
                kill(getpid(), signo);
        } else {
                msgctl(msgid, IPC_RMID, NULL);
                _exit(0);
        }
}


int msg_main() {
        int i, sleep_ret;
        for (i = 1; i < 30; signal(i++, msg_sighandler));
        if (-1 == (msgid = msgget(2987, IPC_CREAT | 0666))) {
                perror("msgget");
                _exit(3);
        }

        while (sleep_ret = usleep(1000), true) {
                if (-1 == sleep_ret) {
                        if (EINTR == errno) goto force_update;
                        msgctl(msgid, IPC_RMID, NULL);
                        perror("usleep");
                        _exit(5);
                }
force_update:
                if (update()) {
                        struct msg_buf msg_buf;
                        msg_buf.mtype = 1;
                        memcpy(msg_buf.mtext, &state, sizeof(state));
                        if (-1 == msgsnd(msgid, &msg_buf,
                            sizeof(msg_buf.mtext), 0)) {
                                perror("msgsnd");
                                msgctl(msgid, IPC_RMID, NULL);
                                _exit(6);
                        }
                }
        }
        return 0;
}


static void mmap_sighandler(int signo) {
        if (signo == SIGUSR1) {
                signal(signo, shm_sighandler);
                kill(getpid(), signo);
        } else {
                munmap(mapaddr, sizeof(struct state));
                shm_unlink(S_FNAME);
                _exit(0);
        }
}


int mmap_main() {
        int i, sleep_ret;
        for (i = 1; i < 30; signal(i++, mmap_sighandler));
        if (-1 == (fd = shm_open(S_FNAME, O_CREAT | O_RDWR, S_IRWXU))) {
                perror("shm_open");
                _exit(3);
        }
        if (-1 == ftruncate(fd, sizeof(struct state))) {
                perror("ftruncate");
                shm_unlink(S_FNAME);                
                _exit(7);
        }
        if (MAP_FAILED == (mapaddr = mmap(NULL, sizeof(struct state),
            PROT_WRITE, MAP_SHARED, fd, 0))) {
                perror("mmap");
                shm_unlink(S_FNAME);                
                _exit(4);
        }

        while (sleep_ret = usleep(1000), true) {
                if (-1 == sleep_ret) {
                        if (EINTR == errno) goto force_update;
                        perror("usleep");
                        munmap(mapaddr, sizeof(struct state));
                        shm_unlink(S_FNAME);
                        _exit(5);
                }
force_update:
                if (update()) {
                        memcpy(mapaddr, &state, sizeof(state));
                }
        }
}
