#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#include "globdef.h"


static void usage(char *name) {
  fprintf(stderr, "Usage: %s <host> <dir ...>\n", name);
  _exit(1);
}


int main(int c, char *v[]) {
  int fd = -1;
  struct sockaddr_in addr;
  struct hostent *hent;
  char ip[20];
  int i;
  char answer[1025];

  if (c < 3) usage(v[0]);

  if (NULL == (hent = gethostbyname(v[1]))) {
    herror("gethostbyname");
    _exit(2);
  }

  if (NULL == (struct in_addr **) hent->h_addr_list) {
    fputs("Cannot find host", stderr);
    _exit(3);
  }
  strcpy(ip, inet_ntoa(*((struct in_addr **) hent->h_addr_list)[0]));

  if (-1 == (fd = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("socket");
    _exit(2);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  inet_aton(ip, &addr.sin_addr);
  if (-1 == connect(fd, (struct sockaddr *) &addr, sizeof(addr))) {
    perror("connect");
  }
  for (i = 2; i < c; ++i) {
    write(fd, v[i], strlen(v[i]));
    write(fd, "\n", sizeof("\n") - 1);
    {
      ssize_t read_len;
      while (0 < (read_len = read(fd, answer, 1))) {
        write(STDIN_FILENO, answer, (size_t) read_len);
        if ('\0' == answer[read_len-1])
          break;
      }
    }
  }
  write(fd, "//close\n", sizeof("//close\n"));
  close(fd);
}
