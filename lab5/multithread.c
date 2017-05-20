#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "globdef.h"

static char *toy;
static size_t len;

static union lock {
        sem_t sem;
        int ipc_sem;
} snc;


static enum sync_type {
        SEM,
        IPC_SEM
} sync_type;


static void init_sync() {
        switch (sync_type) {
        case SEM:
                if (-1 == sem_init(&snc.sem, 0, 1)) {
                        perror("sem_init");
                        _exit(5);
                }
                break;
        case IPC_SEM:
                if (-1 == (snc.ipc_sem
                                = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600))) {
                        perror("semget");
                        _exit(5);
                }
                if (-1 == semctl(snc.ipc_sem, 0, SETVAL, 1)) {
                        perror("semctl");
                        _exit(5);
                }
                break;
        default:
                fputs(stderr, "Synchronization type choosing is broken!\n");
                _exit(-2);
        }
}


static void lock() {
        switch (sync_type) {
        case SEM:
                sem_wait(&snc.sem);
                break;
        case IPC_SEM:
        {
                struct sembuf sops;
                sops.sem_num = 0;
                sops.sem_op = -1;
                sops.sem_flg = 0;
                semop(snc.ipc_sem, &sops, 1);
                break;
        }
        default:
                fputs(stderr, "Synchronization type choosing is broken!\n");
                _exit(-2);
        }
}


static void unlock() {
        switch (sync_type) {
        case SEM:
                sem_post(&snc.sem);
                break;
        case IPC_SEM:
        {
                struct sembuf sops;
                sops.sem_num = 0;
                sops.sem_op = 1;
                sops.sem_flg = 0;
                semop(snc.ipc_sem, &sops, 1);
                break;
        }
        default:
                fputs(stderr, "Synchronization type choosing is broken!\n");
                _exit(-2);
        }
}


static void destroy_sync() {
        switch (sync_type) {
        case SEM:
                sem_destroy(&snc.sem);
                break;
        case IPC_SEM:
                semctl(snc.ipc_sem, 0, IPC_RMID);
                break;
        default:
                fputs(stderr, "Synchronization type choosing is broken!\n");
                _exit(-2);
        }
}


static void* reverse(void *arg) {
        size_t i, j;
        lock();
        for (j = 0; j < 5; ++j) {
                for (i = 0; i < len / 2; ++i) {
                        char c = toy[i];
                        toy[i] = toy[len - i - 1];
                        toy[len - i - 1] = c;
                }
        }
        unlock();
        pthread_exit(NULL);
}


static void* change_register(void *arg) {
        size_t i, j;
        lock();
        for (j = 0; j < 5; ++j) {
                for (i = 0; i < len; ++i) {
                        if (toy[i] >= 'A' && toy[i] <= 'Z') {
                                toy[i] = (toy[i] - 'A') + 'a';
                        } else {
                                toy[i] = (toy[i] - 'a') + 'A';
                        }
                }
        }
        unlock();
        pthread_exit(NULL);
}


static void sighandler(int signo) {
        destroy_sync();
}


int main(int c, char *v[]) {
        int sleep_ret;
        size_t i = 0;
        void* (*funcs[2])(void *) = { reverse, change_register };

        for (sleep_ret = 1; sleep_ret < 30; signal(sleep_ret++, sighandler));

        if (c != 2) {
                fprintf(stderr, "USAGE: %s [sem|ipc]\n", v[0]);
                _exit(-1);
        } else {
                if (!strcmp(v[1], "sem")) {
                        sync_type = SEM;
                } else if (!strcmp(v[1], "ipc")) {
                        sync_type = IPC_SEM;
                } else {
                        fprintf(stderr, "USAGE: %s [sem|ipc]\n", v[0]);
                        _exit(-1);
                }
        }

        if (NULL == (toy = (char *) malloc(57))) {
                perror("malloc");
                _exit(1);
        }
        strcpy(toy, "abcdefghijklmnopqrstuvwxyzabacadefghijklmnopqrstuvwxyz");
        len = strlen(toy);

        init_sync();
        while (sleep_ret = usleep(1000000), i = 1 - i, true) {
                /*pthread_t th;
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                if (pthread_create(&th, NULL, funcs[i], NULL)) {
                        perror("pthread_create");
                        _exit(3);
                }
                if (pthread_join(th, NULL)) {
                        perror("pthread_join");
                        _exit(4);
                }
                */
                pthread_t th1, th2;
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                if (pthread_create(&th1, NULL, reverse, NULL)) {
                        perror("pthread_create");
                        _exit(3);
                }
                if (pthread_create(&th2, NULL, change_register, NULL)) {
                        perror("pthread_create");
                        _exit(3);
                }
                if (pthread_join(th1, NULL)) {
                        perror("pthread_join");
                        _exit(4);
                }
                if (pthread_join(th2, NULL)) {
                        perror("pthread_join");
                        _exit(4);
                }
                puts(toy);
        }
        destroy_sync();

        return 0;
}
