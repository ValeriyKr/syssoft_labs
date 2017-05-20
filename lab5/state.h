#ifndef _STATE_H_
#define _STATE_H_

extern int getloadavg(double loadavg[], int nelem);

typedef int bool_t;
#define true 1
#define false 0


struct state {
        time_t start_time;
        time_t working_time;
        pid_t pid;
        uid_t uid;
        gid_t gid;
        double loadavg[3];
};

#endif /* _STATE_H_ */
