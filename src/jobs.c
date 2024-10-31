#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "jobs.h"

// Define your job list
Job *job_list = NULL;
int next_job_id = 1;

void add_job(pid_t pid, char *command)
{
    Job *new_job = malloc(sizeof(Job));
    new_job->job_id = next_job_id++;
    new_job->pid = pid;
    strncpy(new_job->command, command, sizeof(new_job->command) - 1);
    new_job->command[sizeof(new_job->command) - 1] = '\0';
    new_job->next = job_list;
    job_list = new_job;

    printf("Background job started: [%d] %d %s\n", new_job->job_id, pid, command);
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

void free_jobs()
{
    Job *current = job_list;
    while (current != NULL)
    {
        Job *next_job = current->next;
        free(current);
        current = next_job;
    }
    job_list = NULL;
}

void remove_job(pid_t pid, int job_id)
{
    Job **current = &job_list;
    while (*current != NULL)
    {
        if ((*current)->pid == pid || (*current)->job_id == job_id)
        {
            Job *to_delete = *current;
            *current = (*current)->next;
            free(to_delete);
            return;
        }
        current = &(*current)->next;
    }
}

pid_t get_pid_from_job_id(int job_id)
{
    Job *current = job_list;
    while (current != NULL)
    {
        if (current->job_id == job_id)
        {
            return current->pid;
        }
        current = current->next;
    }
    return -1;
}

void check_completed_jobs()
{
    Job *current = job_list;
    Job *prev = NULL;

    while (current != NULL)
    {
        int status;
        pid_t result = waitpid(current->pid, &status, WNOHANG);

        if (result == -1)
        {
            perror("waitpid error");
            return;
        }
        else if (result > 0)
        {
            printf("Completed: [%d] %d %s\n", current->job_id, current->pid, current->command);

            if (prev == NULL)
            {
                job_list = current->next;
                free(current);
                current = job_list;
            }
            else
            {
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }
}
