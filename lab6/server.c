#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "globdef.h"
#include "queue.h"

static size_t n, k;
static int sockfd = -1;

static struct queue * clients;
static pthread_mutex_t queue_mutex;
static pthread_cond_t client_came;
static size_t pool_size;
static pthread_mutex_t pool_size_mutex;
static size_t free_workers_count;
static pthread_mutex_t free_workers_count_mutex;


static void usage(char *name) {
  fprintf(stderr, "Usage: %s <N> <K>\n", name);
  fputs(", where 1<=N<=20, N<=K\n", stderr);
  _exit(1);
}


static void cleanup() {
  int client;

  while (-1 != (client = queue_dequeue(clients))) {
    close(client);
  }
  queue_destroy(clients);
  close(sockfd);
  pthread_cond_destroy(&client_came);
  pthread_mutex_destroy(&queue_mutex);
  _exit(0);
}


static void sighandler(int s) {
  signal(s, sighandler);
  cleanup();
}


static void do_work(int client) {
  char *buf = NULL;
  do {
    char *new_buf = NULL;
    size_t buf_size = 1024;
    size_t len = 0;
    char c = '\0';

    if (NULL == (buf = (char *) malloc(buf_size))) {
      goto clean;
    }

    while ('\n' != c) {
      if (-1 == read(client, &c, 1)) {
        goto clean;
      }
      buf[len++] = c;
      if ((buf_size - len) <= 2) {
        if (NULL == (new_buf = (char *) realloc(buf, buf_size * 2))) {
          goto clean;
        }
        buf = new_buf;
        buf_size *= 2;
      }
    }

    if (!strcmp(buf, "//close\n")) {
      free(buf);
      return;
    }

    {
      size_t i;
      for (i = 0; i <= len; ++i) {
        if (buf[i] == '\n') {
          buf[i] = '\0';
          len = i;
          break;
        }
      }
    }
    write(client, buf, len);
    write(client, ":\n", sizeof(":\n"));
    {
      DIR *dir;
      struct dirent *ent;
      if (NULL != (dir = opendir(buf))) {
        while (NULL != (ent = readdir(dir))) {
          write(client, "\t", sizeof("\t"));
          write(client, ent->d_name, strlen(ent->d_name));
          write(client, "\n", sizeof("\n"));
        }
        closedir(dir);
      } else {
        write(client, "\t<error>", sizeof("\t<error>"));
      }
    }

    free(buf);
  } while (true);

clean:
  write(client, "server error", sizeof("server error"));
  free(buf);
}


static void * worker(void *argv) {
  int client;
  if (argv) { }
  while (true) {
    pthread_mutex_lock(&free_workers_count_mutex);
    ++free_workers_count;
    pthread_mutex_unlock(&free_workers_count_mutex);

    pthread_cond_wait(&client_came, &queue_mutex);
    client = queue_dequeue(clients);
    pthread_mutex_unlock(&queue_mutex);
    if (-1 == client) {
      pthread_mutex_lock(&pool_size_mutex);
      if (pool_size > n) {
        --pool_size;
        pthread_mutex_unlock(&pool_size_mutex);
        break;
      }
      pthread_mutex_unlock(&pool_size_mutex);
    }
    pthread_mutex_lock(&free_workers_count_mutex);
    --free_workers_count;
    pthread_mutex_unlock(&free_workers_count_mutex);
    do_work(client);
    close(client);
  }
  pthread_mutex_lock(&free_workers_count_mutex);
  --free_workers_count;
  pthread_mutex_unlock(&free_workers_count_mutex);
  pthread_exit(NULL);
}


static void service_client() {
  pthread_mutex_lock(&free_workers_count_mutex);
  if (0 == free_workers_count) {
    pthread_t th;

    pthread_mutex_unlock(&free_workers_count_mutex);

    pthread_mutex_lock(&pool_size_mutex);
    if (pool_size < k) {
      ++pool_size;
      pthread_mutex_unlock(&pool_size_mutex);
      if (pthread_create(&th, NULL, worker, NULL)) {
        perror("pthread_create");
        _exit(8);
      }

      if (pthread_detach(th)) {
        perror("pthread_detach");
        _exit(9);
      }
    } else {
      pthread_mutex_unlock(&pool_size_mutex);
    }
  } else {
    pthread_mutex_unlock(&free_workers_count_mutex);
  }
  pthread_cond_signal(&client_came);
  sched_yield();
  pthread_cond_signal(&client_came);
}


int main(int c, char *v[]) {
  struct sockaddr_in addr;
  int opt = 1;

  if (c != 3) {
    usage(v[0]);
  }
  n = (size_t) atoll(v[1]);
  k = (size_t) atoll(v[2]);
  if (! (1 <= n && n <= 20 && n <= k)) {
    usage(v[0]);
  }
  
  {
    int i;
    for (i = 1; i < 30; ++i, signal(i, sighandler));
  }

  if (NULL == (clients = queue_init())) {
    perror("malloc");
    _exit(6);
  }

  if (0 != pthread_mutex_init(&queue_mutex, NULL)) {
    perror("pthread_mutex_init");
    _exit(7);
  }

  if (0 != pthread_cond_init(&client_came, NULL)) {
    perror("pthread_cond_init");
    _exit(7);
  }
  
  if (0 != pthread_mutex_init(&pool_size_mutex, NULL)) {
    perror("pthread_mutex_init");
    _exit(7);
  }

  {
    size_t i;
    for (i = 0; i < n; ++i) {
      pthread_t th;
      if (pthread_create(&th, NULL, worker, NULL)) {
        perror("pthread_create");
        _exit(8);
      }

      if (pthread_detach(th)) {
        perror("pthread_detach");
        _exit(9);
      }
    }
    pool_size = n;
  }

  if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("socket");
    _exit(2);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt))) {
    perror("setsockopt");
    _exit(2);
  }

  if (-1 == bind(sockfd, (struct sockaddr *) &addr, sizeof(addr))) {
    perror("bind");
    _exit(3);
  }

  if (-1 == listen(sockfd, 50)) {
    perror("listen");
    _exit(4);
  }

  while (true) {
    int fd;
    struct sockaddr_in cli_addr;
    size_t cli_addr_len = sizeof(cli_addr);
    if (-1 == (fd = accept(sockfd, (struct sockaddr *) &cli_addr,
            (socklen_t *) &cli_addr_len))) {
      perror("accept");
      close(sockfd);
      _exit(5);
    }
    pthread_mutex_lock(&queue_mutex);
    queue_enqueue(clients, fd);
    pthread_mutex_unlock(&queue_mutex);
    service_client();
  }
}
