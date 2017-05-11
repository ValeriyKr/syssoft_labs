#include <sys/types.h>
#include <stdlib.h>

#include "globdef.h"
#include "job.h"


static size_t jobs_capacity = 255;
static pid_t *jobs;


size_t add_job(pid_t pid) {
        if (NULL == jobs) {
                TRY_ALLOC(jobs = (pid_t*) 
                                calloc(jobs_capacity + 1, sizeof(pid_t)));
        }

        size_t i;
        for (i = 1; i <= jobs_capacity && 0 != jobs[i]; ++i);
        if (i > jobs_capacity) {
                size_t j;

                jobs_capacity *= 2;
                TRY_ALLOC(jobs = (pid_t*)
                                realloc(jobs, sizeof(pid_t)*jobs_capacity+1));
                for (j = i; j <= jobs_capacity; ++j) {
                        jobs[j] = 0;
                }
        }
        jobs[i] = pid;
        return i;
}


pid_t get_job(size_t id) {
        if (NULL == jobs || id > jobs_capacity) return 0;
        return jobs[id];
}


size_t get_job_id(pid_t pid) {
        size_t i;

        if (NULL == jobs) return 0;
        for (i = 1; i <= jobs_capacity && jobs[i] != pid; ++i);
        if (i > jobs_capacity || jobs[i] != pid) return 0;
        return i;
}


void delete_job_by_id(size_t id) {
        if (NULL == jobs || id > jobs_capacity) return;
        jobs[id] = 0;
}


void delete_job_by_pid(pid_t pid) {
        size_t i;

        if (NULL == jobs) return;
        for (i = 1; i <= jobs_capacity; ++i) {
                if (pid == jobs[i]) {
                        jobs[i] = 0;
                        return;
                }
        }
}
