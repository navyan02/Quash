
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jobs.h"

static Job *job_list = NULL;
int next_job_id = 1; // Define next_job_id here

void add_job(pid_t pid, char *command)
{
    Job *new_job = (Job *)malloc(sizeof(Job));
    new_job->job_id = next_job_id++;
    new_job->pid = pid;
    strncpy(new_job->command, command, BUFFER_SIZE);
    new_job->next = job_list;
    job_list = new_job;
}

void remove_job(pid_t pid)
{
    Job *current = job_list;
    Job *previous = NULL;

    while (current != NULL)
    {
        if (current->pid == pid)
        {
            if (previous == NULL)
            {
                job_list = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void list_jobs()
{
    Job *current = job_list;
    while (current != NULL)
    {
        printf("[%d] %d %s\n", current->job_id, current->pid, current->command);
        current = current->next;
    }
}
