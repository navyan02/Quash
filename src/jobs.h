#ifndef JOBS_H
#define JOBS_H
#define BUFFER_SIZE 1024

typedef struct Job
{
    int job_id;
    pid_t pid;
    char command[BUFFER_SIZE];
    struct Job *next;
} Job;

extern int next_job_id; // Declare next_job_id here

void add_job(pid_t pid, char *command);
void remove_job(pid_t pid);
void list_jobs();

#endif // JOBS_H
