#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>

/**
 * \file job.h
 * \brief Declares job-control functions of kcsh.
 */

/**
 * Adds background job to jobs' list.
 *
 * \param pid PID of job.
 * \return id of added job.
 */
size_t add_job(pid_t pid);


/**
 * Returns PID of job by its id.
 *
 * \param id id of job.
 * \return PID of job or 0 if no job has been found.
 */
pid_t get_job(size_t id);


/**
 * Returns job id by PID.
 *
 * \param pid PID of job to found.
 * \return job id or 0 if no job has been found.
 */
size_t get_job_id(pid_t pid);


/**
 * Deletes job by its id.
 *
 * \param id id of job to delete.
 */
void delete_job_by_id(size_t id);


/**
 * Deletes job by its PID.
 *
 * \param pid PID of job to delete.
 */
void delete_job_by_pid(pid_t pid);

#endif /* _JOB_H_ */
