#include <unistd.h>
#include <sys/types.h>
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
} snc;


static void init_sync() {
        sem_init(&snc.sem, 0, 1);
}


static void lock() {
        sem_wait(&snc.sem);
}


static void unlock() {
        sem_post(&snc.sem);
}


static void destroy_sync() {
        sem_destroy(&snc.sem);
}


static void* reverse(void *arg) {
        size_t i;
        lock();
        for (i = 0; i < len / 2; ++i) {
                char c = toy[i];
                toy[i] = toy[len - i - 1];
                toy[len - i - 1] = c;
        }
        unlock();
        pthread_exit(NULL);
}


static void* change_register(void *arg) {
        size_t i;
        lock();
        for (i = 0; i < len; ++i) {
                if (toy[i] >= 'A' && toy[i] <= 'Z') {
                        toy[i] = (toy[i] - 'A') + 'a';
                } else {
                        toy[i] = (toy[i] - 'a') + 'A';
                }
        }
        unlock();
        pthread_exit(NULL);
}


int main() {
        int sleep_ret;
        size_t i = 0;
        void* (*funcs[2])(void *) = { reverse, change_register };

        if (NULL == (toy = (char *) malloc(27))) {
                perror("malloc");
                _exit(1);
        }
        strcpy(toy, "abcdefghijklmnopqrstuvwxyz");
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
