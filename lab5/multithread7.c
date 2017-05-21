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
static int tmain;
static int tcount;
static int treverse;
static int tcap;


static pthread_rwlock_t rwlock;


static void init_sync() {
        if (0 != pthread_rwlock_init(&rwlock, NULL)) {
                perror("pthread_rwlock_init");
                _exit(5);
        }
}


static void rlock() {
        if (0 != pthread_rwlock_rdlock(&rwlock)) {
                perror("pthread_rdlock_rdlock");
                _exit(5);
        }
}


static void wlock() {
        if (0 != pthread_rwlock_wrlock(&rwlock)) {
                perror("pthread_wrlock_rdlock");
                _exit(5);
        }
}


static void unlock() {
        if (0 != pthread_rwlock_unlock(&rwlock)) {
                perror("pthread_rwlock_unlock");
                _exit(5);
        }
}


static void destroy_sync() {
        if (0 != pthread_rwlock_destroy(&rwlock)) {
                perror("pthread_rwlock_destroy");
                _exit(5);
        }
}


static void* reverse(void *arg) {
        size_t i, j;
        int sleep_ret;
        while (sleep_ret = usleep(treverse), true) {
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                wlock();
                for (j = 0; j < 5; ++j) {
                        for (i = 0; i < len / 2; ++i) {
                                char c = toy[i];
                                toy[i] = toy[len - i - 1];
                                toy[len - i - 1] = c;
                        }
                }
                unlock();
        }
        pthread_exit(NULL);
}


static void* change_register(void *arg) {
        size_t i, j;
        int sleep_ret;
        while (sleep_ret = usleep(tcount), true) {
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                wlock();
                for (j = 0; j < 5; ++j) {
                        for (i = 0; i < len; ++i) {
                                if (toy[i] >= 'A' && toy[i] <= 'Z') {
                                        toy[i] = (toy[i] - 'A') + 'a';
                                } else if (toy[i] >= 'a' && toy[i] <= 'z') {
                                        toy[i] = (toy[i] - 'a') + 'A';
                                }
                        }
                }
                unlock();
        }
        pthread_exit(NULL);
}


static void* count_capitalized(void *argv) {
        size_t i;
        int sleep_ret;
        while (sleep_ret = usleep(treverse), true) {
                size_t cnt = 0;
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                rlock();
                for (i = 0; i < len; ++i) {
                        cnt += (toy[i] >= 'A' && toy[i] <= 'Z');
                }
                unlock();
                flockfile(stdout);
                printf("%lu\n", cnt);
                funlockfile(stdout);
        }
        pthread_exit(NULL);
}


static void sighandler(int signo) {
        destroy_sync();
        _exit(0);
}


static void usage(char *v) {
        fprintf(stderr, "USAGE: %s <tmain> <tcount> <treverse> <tcap>\n", v);
        _exit(-1);
}


int main(int c, char *v[]) {
        int sleep_ret;
        size_t i = 0;
        pthread_t th1, th2, th3;

        for (sleep_ret = 1; sleep_ret < 30; signal(sleep_ret++, sighandler));

        if (c != 5) {
                usage(*v);
        }

        tmain = atoi(v[1]);
        tcount = atoi(v[2]);
        treverse = atoi(v[3]);
        tcap = atoi(v[4]);

        if (NULL == (toy = (char *) malloc(57))) {
                perror("malloc");
                _exit(1);
        }
        strcpy(toy, "The Source String is The Greatest String of the World!");
        len = strlen(toy);

        if (pthread_create(&th1, NULL, reverse, NULL)) {
                perror("pthread_create");
                _exit(3);
        }
        if (pthread_create(&th2, NULL, change_register, NULL)) {
                perror("pthread_create");
                _exit(3);
        }
        if (pthread_create(&th3, NULL, count_capitalized, NULL)) {
                perror("pthread_create");
                _exit(3);
        }
        init_sync();
        while (sleep_ret = usleep(tmain), i = 1 - i, true) {
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                flockfile(stdout);
                puts(toy);
                funlockfile(stdout);
        }
        if (pthread_join(th3, NULL)) {
                perror("pthread_join");
                _exit(4);
        }
        if (pthread_join(th2, NULL)) {
                perror("pthread_join");
                _exit(4);
        }
        if (pthread_join(th1, NULL)) {
                perror("pthread_join");
                _exit(4);
        }
        destroy_sync();

        return 0;
}
