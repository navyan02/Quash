#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "jobs.h"

// Define your job list
Job *job_list = NULL;
int next_job_id = 1;

// Function to add a job
void add_job(pid_t pid, char *command)
{
	Job *new_job = malloc(sizeof(Job));
	new_job->job_id = next_job_id++; // Assign a unique job ID
	new_job->pid = pid;
	strncpy(new_job->command, command, sizeof(new_job->command) - 1);
	new_job->command[sizeof(new_job->command) - 1] = '\0'; // Ensure null-termination
	new_job->next = job_list; // Add new job at the start of the list
	job_list = new_job;

	printf("Background job started: [%d] %d %s\n", new_job->job_id, pid, command);
}

// Function to list jobs
void list_jobs()
{
	Job *current = job_list;
	while (current != NULL)
	{
    	printf("[%d] %d %s\n", current->job_id, current->pid, current->command);
    	current = current->next;
	}
}

// Function to free jobs
void free_jobs()
{
	Job *current = job_list;
	while (current != NULL)
	{
    	Job *next_job = current->next; // Save the next job
    	free(current);             	// Free the current job
    	current = next_job;        	// Move to the next job
	}
	job_list = NULL; // Clear the job list
}

// Function to remove a job
void remove_job(pid_t pid)
{
	Job **current = &job_list;
	while (*current != NULL)
	{
    	if ((*current)->pid == pid)
    	{
        	Job *to_delete = *current;
        	*current = (*current)->next; // Remove the job from the list
        	free(to_delete);         	// Free the memory
        	return;
    	}
    	current = &(*current)->next; // Move to the next job
	}
}

// Function to check for completed jobs
void check_completed_jobs()
{
	Job *current = job_list;
	Job *prev = NULL;

	while (current != NULL)
	{
    	int status;
    	pid_t result = waitpid(current->pid, &status, WNOHANG); // Non-blocking wait

    	if (result == -1)
    	{
        	perror("waitpid error");
        	return;
    	}
    	else if (result > 0) // Job has completed
    	{
        	printf("Completed: [%d] %d %s\n", current->job_id, current->pid, current->command);

        	// Remove the job from the list
        	if (prev == NULL) // Removing the head
        	{
            	job_list = current->next;
            	free(current);
            	current = job_list; // Move to the next job
        	}
        	else
        	{
            	prev->next = current->next; // Bypass the current job
            	free(current);
            	current = prev->next; // Move to the next job
        	}
    	}
    	else
    	{
        	prev = current;
        	current = current->next; // Move to the next job
    	}
	}
}
