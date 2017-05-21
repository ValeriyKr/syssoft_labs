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
static int treverse;
static int tcap;


static pthread_mutex_t mutex;


static void init_sync() {
        if (0 != pthread_mutex_init(&mutex, NULL)) {
                perror("pthread_mutex_init");
                _exit(5);
        }
}


static void lock() {
        if (0 != pthread_mutex_lock(&mutex)) {
                perror("pthread_mutex_lock");
                _exit(5);
        }
}


static void unlock() {
        if (0 != pthread_mutex_unlock(&mutex)) {
                perror("pthread_mutex_unlock");
                _exit(5);
        }
}


static void destroy_sync() {
        if (0 != pthread_mutex_destroy(&mutex)) {
                perror("pthread_mutex_destroy");
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
                lock();
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
        while (sleep_ret = usleep(tcap), true) {
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
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
        }
        pthread_exit(NULL);
}


static void sighandler(int signo) {
        destroy_sync();
}


static void usage(char *v) {
        fprintf(stderr, "USAGE: %s <tmain> <treverse> <tcap>\n", v);
        _exit(-1);
}


int main(int c, char *v[]) {
        int sleep_ret;
        size_t i = 0;

        for (sleep_ret = 1; sleep_ret < 30; signal(sleep_ret++, sighandler));

        if (c != 4) {
                usage(*v);
        }

        tmain = atoi(v[1]);
        treverse = atoi(v[2]);
        tcap = atoi(v[3]);

        if (NULL == (toy = (char *) malloc(57))) {
                perror("malloc");
                _exit(1);
        }
        strcpy(toy, "abcdefghijklmnopqrstuvwxyzabacadefghijklmnopqrstuvwxyz");
        len = strlen(toy);

        pthread_t th1, th2;
        if (pthread_create(&th1, NULL, reverse, NULL)) {
                perror("pthread_create");
                _exit(3);
        }
        if (pthread_create(&th2, NULL, change_register, NULL)) {
                perror("pthread_create");
                _exit(3);
        }
        init_sync();
        while (sleep_ret = usleep(tmain), i = 1 - i, true) {
                if (-1 == sleep_ret) {
                        perror("usleep");
                        _exit(2);
                }
                lock();
                puts(toy);
                unlock();
        }
        if (pthread_join(th1, NULL)) {
                perror("pthread_join");
                _exit(4);
        }
        if (pthread_join(th2, NULL)) {
                perror("pthread_join");
                _exit(4);
        }
        destroy_sync();

        return 0;
}
