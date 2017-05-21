#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>

#include "state.h"


int msg_main(void);
int mmap_main(void);


int main() {
        struct state state;
        int fd;
        struct sockaddr_un addr;

        if (-1 == (fd = socket(AF_UNIX, SOCK_STREAM, 0))) {
                perror("soucket");
                _exit(1);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCK_FNAME, sizeof(addr.sun_path) - 1);

        if (-1 == connect(fd, (struct sockaddr *) &addr, sizeof(addr))) {
                perror("connect");
                _exit(1);
        }

        if (sizeof(state) != read(fd, &state, sizeof(state))) {
                perror("read");
                _exit(2);
        }
        close(fd);

        printf("Working time: %lu\nSrv pid: %d\nSrv uid: %u\nSrv gid: %u\n",
                        state.working_time, state.pid, state.uid,
                        state.gid);
        printf("Load average for 1 minute: %lf\n", state.loadavg[0]);
        printf("Load average for 5 minutes: %lf\n", state.loadavg[1]);
        printf("Load average for 15 minutes: %lf\n", state.loadavg[2]);

        return 0;
}

