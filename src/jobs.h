#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#define BUFFER_SIZE 1024

typedef struct Job
{
    int job_id;                // Unique job ID
    pid_t pid;                 // Process ID of the job
    char command[BUFFER_SIZE]; // Command associated with the job
    struct Job *next;          // Pointer to the next job in the list
} Job;

int add_job(pid_t pid, char *command);
void remove_job(const char *input);
void list_jobs();
void free_jobs();
void check_completed_jobs();

#endif // JOBS_H
