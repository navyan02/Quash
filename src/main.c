#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For fork(), exec(), and process management
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

// Function declarations
void run_quash();
void handle_exit(char *input);
void run_echo(char *input); // Declare run_echo here
void cd(char *path);        // Declare cd function here
void run_echo(char *input);       // Declare run_echo here
void builtin_export(char *input); // Declare builtin_export here
void pwd();                       // Declare pwd here
void add_job(pid_t pid, const char *command); // Add job handling
void list_jobs(); // Declare jobs listing function
void remove_job(pid_t pid); // Declare job removal function

int main()
{
    printf("Welcome to Quash!\n");
    run_quash();
    return 0;
}

// Main loop for Quash
void run_quash()
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("[QUASH]$ "); // Shell prompt
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            perror("Error reading input");
            continue;
        }

        // Strip newline character from input
        buffer[strcspn(buffer, "\n")] = 0;

        // Handle the "exit" or "quit" commands
        handle_exit(buffer);

        // Remove leading '$' characters from the input
        char *command = buffer; // Use a new pointer for command processing
        while (*command == '$' || *command == ' ') {
            command++; // Move the pointer to the right
        }

        // If the input starts with "echo", call the run_echo() function
        if (strncmp(command, "echo", 4) == 0)
        {
            run_echo(command);
        }
        // If the input starts with "export", call the export() function
        else if (strncmp(command, "export", 6) == 0)
        {
            builtin_export(command);
        }
        // If the input starts with "pwd", call the pwd() function
        else if (strncmp(buffer, "pwd", 3) == 0)
        {
            pwd();
        }
        // If the input starts with "cd", call the cd() function
        else if (strncmp(command, "cd", 2) == 0)
        {
            char *path = command + 3;  // Get the path (skip "cd ")
            cd(path); // Call the cd function
        }
        // If the input starts with "jobs", call the list_jobs() function
        else if (strncmp(command, "jobs", 4) == 0)
        {
            list_jobs();
        }
        // If the input is a background process (ends with &), handle it
        else if (command[strlen(command) - 1] == '&')
        {
            // Remove '&' from the command string
            command[strlen(command) - 1] = '\0';
            
            // Fork a process to run the command
            pid_t pid = fork();
            if (pid < 0) {
                perror("Failed to fork");
            } else if (pid == 0) {
                // In child process
                execlp("/bin/sh", "sh", "-c", command, (char *) NULL);
                perror("Execution failed");
                exit(EXIT_FAILURE);
            } else {
                // In parent process, add the job
                add_job(pid, command);
                printf("Background job started: [%d] %d %s &\n", job_count, pid, command);
            }
        }
        // Handle unknown commands
        else
        {
            printf("Command not recognized: %s\n", command);
        }

        // Check if any background job has finished and remove it
        int status;
        pid_t finished_pid;
        while ((finished_pid = waitpid(-1, &status, WNOHANG)) > 0) {
            remove_job(finished_pid);
        }
    }
}

// Handle exit or quit command
void handle_exit(char *input)
{
    if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
    {
        printf("Exiting Quash...\n");
        exit(0);
    }
}
