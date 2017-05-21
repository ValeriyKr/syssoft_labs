#ifndef _STATE_H_
#define _STATE_H_

#include "globdef.h"

extern int getloadavg(double loadavg[], int nelem);


#define S_FNAME "/kk2987"
#define SOCK_FNAME "/tmp/kk2987"


struct state {
        time_t start_time;
        time_t working_time;
        pid_t pid;
        uid_t uid;
        gid_t gid;
        double loadavg[3];
};


struct msg_buf {
        long mtype;
        char mtext[sizeof(struct state)];
};

#endif /* _STATE_H_ */
