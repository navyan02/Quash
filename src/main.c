#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pipes.h"
#include "jobs.h"
#include <sys/wait.h>
#include <signal.h> // Include for signal handling
#include <fcntl.h>

#define BUFFER_SIZE 1024

// Function declarations
void run_quash();
int handle_exit(char *input);
void run_echo(char *input);
void cd(char *path);
void builtin_export(char *input);
void pwd();
void execute_command(char *cmd, int background);
void add_job(pid_t pid, char *command);
void remove_job(pid_t pid);
void list_jobs();
void free_jobs(); // Declaration for the cleanup function
void execute_pipes(char ***cmds);

int main()
{
    printf("Welcome to Quash!\n");
    run_quash();
    free_jobs(); // Cleanup jobs before exiting
    return 0;
}

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
        if (handle_exit(buffer))
        {
            continue; // If exit was called, continue to the next iteration (will exit)
        }

        // Remove leading '$' characters from the input
        char *command = buffer; // Use a new pointer for command processing
        while (*command == '$' || *command == ' ')
        {
            command++; // Move the pointer to the right
        }

        // Check for background command
        int background = (command[strlen(command) - 1] == '&');
        if (background)
        {
            command[strlen(command) - 1] = '\0'; // Remove the '&' symbol
        }

        // Check for pipes and redirection symbols
        if (strchr(command, '|') != NULL)
        {
            // Split the command into parts
            char *cmds[BUFFER_SIZE];
            int i = 0;
            cmds[i] = strtok(command, "|");
            while (cmds[i] != NULL)
            {
                cmds[++i] = strtok(NULL, "|");
            }

            // Split each command into arguments
            char **cmd_args[BUFFER_SIZE];
            for (int j = 0; j < i; j++)
            {
                cmd_args[j] = malloc(BUFFER_SIZE * sizeof(char *));
                int k = 0;
                cmd_args[j][k] = strtok(cmds[j], " ");
                while (cmd_args[j][k] != NULL)
                {
                    k++;
                    cmd_args[j][k] = strtok(NULL, " ");
                }
            }
            cmd_args[i] = NULL; // Null-terminate the array of commands

            // Execute the piped commands
            execute_pipes(cmd_args);

            // Free allocated memory
            for (int j = 0; j < i; j++)
            {
                free(cmd_args[j]);
            }
        }
        else
        {
            // Check for redirection symbols
            if (strchr(command, '>') != NULL || strchr(command, '<') != NULL)
            {
                execute_command(command, background);
            }
            else if (strncmp(command, "echo", 4) == 0)
            {
                run_echo(command);
            }
            else if (strncmp(command, "export", 6) == 0)
            {
                builtin_export(command);
            }
            else if (strncmp(command, "pwd", 3) == 0)
            {
                pwd();
            }
            else if (strncmp(command, "cd", 2) == 0)
            {
                char *path = command + 3; // Get the path (skip "cd ")
                cd(path);                 // Call the cd function
            }
            else if (strcmp(command, "jobs") == 0)
            {
                list_jobs();
            }
            else if (strncmp(command, "kill", 4) == 0)
            {
                char *pid_str = command + 5; // Get the PID (skip "kill ")
                pid_t pid = atoi(pid_str);   // Convert string to PID

                // Send SIGTERM to the specified PID
                if (kill(pid, SIGTERM) == 0)
                {
                    printf("Sent kill signal to job [%d]\n", pid);
                    remove_job(pid); // Remove job from the job list
                }
                else
                {
                    perror("Failed to kill the job");
                }
            }
            else
            {
                execute_command(command, background);
            }
        }

        check_completed_jobs();
    }
}

// Modify handle_exit to return a value
int handle_exit(char *input)
{
    if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
    {
        printf("Exiting Quash...\n");
        exit(0); // Indicate that exit was called
    }
    return 0; // Indicate that exit was not called
}

void execute_command(char *cmd, int background)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        char *args[BUFFER_SIZE];
        char *input_file = NULL;
        char *output_file = NULL;
        int append_mode = 0;

        // Tokenize the command and check for redirection
        char *token = strtok(cmd, " ");
        int i = 0;
        while (token != NULL)
        {
            // Check for input redirection
            if (strcmp(token, "<") == 0)
            {
                input_file = strtok(NULL, " ");
            }
            // Check for output redirection
            else if (strcmp(token, ">") == 0)
            {
                output_file = strtok(NULL, " ");
            }
            // Check for append redirection
            else if (strcmp(token, ">>") == 0)
            {
                append_mode = 1;
                output_file = strtok(NULL, " ");
            }
            else
            {
                args[i++] = token; // Add to args if not a redirection token
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the args array

        // Handle input redirection
        if (input_file)
        {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO); // Redirect standard input
            close(input_fd);
        }

        // Handle output redirection
        if (output_file)
        {
            int output_fd = open(output_file, O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC), 0644);
            if (output_fd < 0)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        execvp(args[0], args);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        if (background)
        {
            add_job(pid, cmd);
        }
        else
        {
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        perror("fork");
    }
}
