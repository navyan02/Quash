#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_JOBS 100

typedef struct Job {
    int job_id;           // Unique Job ID
    pid_t pid;            // PID of the background process
    char command[256];    // Command used to invoke the job
    int running;          // Status of the job (1 if running, 0 if completed)
} Job;

Job jobs[MAX_JOBS];       // Array to store background jobs
int num_jobs = 0;         // Count of jobs

// Function to add a job to the jobs list
void add_job(pid_t pid, char *cmd) {
    if (num_jobs >= MAX_JOBS) {
        fprintf(stderr, "Max job limit reached!\n");
        return;
    }

    jobs[num_jobs].job_id = num_jobs + 1;
    jobs[num_jobs].pid = pid;
    strncpy(jobs[num_jobs].command, cmd, 255);
    jobs[num_jobs].command[255] = '\0';  // Ensure null-termination
    jobs[num_jobs].running = 1;  // Mark job as running
    num_jobs++;
}

// Function to remove a completed job
void remove_job(pid_t pid) {
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].pid == pid) {
            jobs[i].running = 0;  // Mark job as completed
            break;
        }
    }
}

// Function to print currently running jobs
void print_jobs() {
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].running) {
            printf("[%d] %d %s &\n", jobs[i].job_id, jobs[i].pid, jobs[i].command);
        }
    }
}

// Function to check background jobs
void check_jobs() {
    int status;
    pid_t pid;

    // Use waitpid with WNOHANG to avoid blocking
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // If a background process has completed, remove it from the list
        remove_job(pid);
    }
}

// Function to execute a command with pipe support
void execute_command(char *cmd, int background) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        int pipefd[2];
        char *command1, *command2;

        // Check for pipes
        if (strchr(cmd, '|')) {
            // Split the command on the pipe
            command1 = strtok(cmd, "|");
            command2 = strtok(NULL, "|");

            // Create pipe
            if (pipe(pipefd) == -1) {
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }

            // First child process
            pid_t pid1 = fork();
            if (pid1 == -1) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid1 == 0) {
                // Redirect output to pipe
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);  // Close read end
                close(pipefd[1]);  // Close write end

                char *args[100];
                char *token = strtok(command1, " \t\n");
                int i = 0;
                while (token != NULL) {
                    args[i++] = token;
                    token = strtok(NULL, " \t\n");
                }
                args[i] = NULL;

                execvp(args[0], args);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            } else {
                // Second child process
                pid_t pid2 = fork();
                if (pid2 == -1) {
                    perror("fork failed");
                    exit(EXIT_FAILURE);
                }

                if (pid2 == 0) {
                    // Redirect input from pipe
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[1]);  // Close write end
                    close(pipefd[0]);  // Close read end

                    char *args[100];
                    char *token = strtok(command2, " \t\n");
                    int i = 0;
                    while (token != NULL) {
                        args[i++] = token;
                        token = strtok(NULL, " \t\n");
                    }
                    args[i] = NULL;

                    execvp(args[0], args);
                    perror("execvp failed");
                    exit(EXIT_FAILURE);
                } else {
                    // Parent process
                    close(pipefd[0]);  // Close read end
                    close(pipefd[1]);  // Close write end
                    waitpid(pid1, NULL, 0);
                    waitpid(pid2, NULL, 0);
                }
            }
        } else {
            // Execute without pipe
            char *args[100];
            char *token = strtok(cmd, " \t\n");
            int i = 0;
            while (token != NULL) {
                args[i++] = token;
                token = strtok(NULL, " \t\n");
            }
            args[i] = NULL;

            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        if (background) {
            // Add job to the jobs array
            add_job(pid, cmd);
            printf("Background job started: [%d] %d %s &\n", num_jobs, pid, cmd);
        } else {
            // Wait for the foreground job to complete
            waitpid(pid, NULL, 0);
        }
    }
}

// Function to handle the jobs command
void handle_jobs_command() {
    // Check for completed jobs before printing
    check_jobs();
    // Print all currently running background jobs
    print_jobs();
}