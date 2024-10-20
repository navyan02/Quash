#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_JOBS 100
#define COMMAND_LEN 256

// Structure to store information about background jobs
typedef struct Job {
    int jobID;
    pid_t pid;
    char command[COMMAND_LEN];
} Job;

// Array to store background jobs
Job jobs[MAX_JOBS];
int job_count = 0;

// Function to add a new job to the jobs list
void add_job(pid_t pid, const char *command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].jobID = job_count + 1;  // Assign a unique job ID starting from 1
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, COMMAND_LEN - 1);
        jobs[job_count].command[COMMAND_LEN - 1] = '\0';  // Ensure null-termination
        job_count++;
    } else {
        printf("Max job limit reached!\n");
    }
}

// Function to remove a job from the jobs list once it finishes
void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            // Shift remaining jobs to fill the gap
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            job_count--;
            break;
        }
    }
}

// Function to list all currently running jobs
void list_jobs() {
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %d %s &\n", jobs[i].jobID, jobs[i].pid, jobs[i].command);
    }
}

