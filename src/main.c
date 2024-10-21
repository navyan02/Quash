#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pipes.h"
#include "jobs.h"

#define BUFFER_SIZE 1024

// Function declarations
void run_quash();
int handle_exit(char *input);
void run_echo(char *input);
void cd(char *path);
void builtin_export(char *input);
void pwd();
void execute_command(char *cmd, int background);
void execute_pipe(char **cmd1, char **cmd2);
void execute_pipes(char ***cmds);
void add_job(pid_t pid, char *command);
void remove_job(pid_t pid);
void list_jobs();

int main()
{
    printf("Welcome to Quash!\n");
    run_quash();
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

        // Check for background command
        int background = (command[strlen(command) - 1] == '&');
        if (background)
        {
            command[strlen(command) - 1] = '\0'; // Remove the '&' symbol
        }

        // Check for pipe symbol
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
                    cmd_args[j][++k] = strtok(NULL, " ");
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
            else if (strncmp(command, "pwd", 3) == 0)
            {
                pwd();
            }
            // If the input starts with "cd", call the cd() function
            else if (strncmp(command, "cd", 2) == 0) // Check only the first 2 characters
            {
                char *path = command + 3; // Get the path (skip "cd ")
                cd(path);                 // Call the cd function
            }
            // Handle the "jobs" command
            else if (strcmp(command, "jobs") == 0)
            {
                list_jobs();
            }
            // Handle other unknown commands
            else
            {
                execute_command(command, background);
            }
        }
    }
}

// Modify handle_exit to return a value
int handle_exit(char *input)
{
    if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
    {
        printf("Exiting Quash...\n");
        exit(0); // You can use exit directly here too
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
        int i = 0;
        args[i] = strtok(cmd, " ");
        while (args[i] != NULL)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process
        if (background)
        {
            add_job(pid, cmd);
            printf("Background job started: [%d] %d %s\n", next_job_id - 1, pid, cmd);
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
