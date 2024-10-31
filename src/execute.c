#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

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
int add_job(pid_t pid, char *command);
void remove_job(const char *input);
void list_jobs();
void free_jobs();
void execute_pipes(char ***cmds);
int handle_builtin(char *command);

int main()
{
    printf("Welcome to Quash!\n");
    run_quash();
    free_jobs();
    return 0;
}

void run_quash()
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        check_completed_jobs();

        printf("[QUASH]$ ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            perror("Error reading input");
            continue;
        }

        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        if (handle_exit(buffer))
        {
            continue;
        }

        char *command = buffer;
        while (*command == '$' || *command == ' ')
            command++; // Remove extra spaces or '$'

        int background = (command[strlen(command) - 1] == '&');
        if (background)
            command[strlen(command) - 1] = '\0';

        if (handle_builtin(command))
        {
            continue;
        }

        if (strchr(command, '|') != NULL)
        {
            char *cmds[BUFFER_SIZE];
            int i = 0;
            cmds[i] = strtok(command, "|");
            while (cmds[i] != NULL)
                cmds[++i] = strtok(NULL, "|");

            char **cmd_args[BUFFER_SIZE];
            for (int j = 0; j < i; j++)
            {
                cmd_args[j] = malloc(BUFFER_SIZE * sizeof(char *));
                int k = 0;
                cmd_args[j][k] = strtok(cmds[j], " ");
                while (cmd_args[j][k] != NULL)
                    cmd_args[j][++k] = strtok(NULL, " ");
            }
            cmd_args[i] = NULL;

            execute_pipes(cmd_args);

            for (int j = 0; j < i; j++)
                free(cmd_args[j]);
        }
        else
        {
            execute_command(command, background);
        }
    }
}

int handle_builtin(char *command)
{
    int background = (command[strlen(command) - 1] == '&');
    if (background)
    {
        command[strlen(command) - 1] = '\0';
    }
    if (strncmp(command, "echo", 4) == 0)
    {
        run_echo(command);
        return 1;
    }
    else if (strncmp(command, "export", 6) == 0)
    {
        builtin_export(command);
        return 1;
    }
    else if (strncmp(command, "pwd", 3) == 0)
    {
        pwd();
        return 1;
    }
    else if (strncmp(command, "cd", 2) == 0)
    {
        cd(command + 3);
        return 1;
    }
    else if (strcmp(command, "jobs") == 0)
    {
        list_jobs();
        return 1;
    }
    else if (strncmp(command, "kill", 4) == 0)
    {
        char *target_str = command + 5;
        if (target_str[0] == '%')
        {
            remove_job(target_str);
        }
        else
        {
            pid_t pid = atoi(target_str);
            if (kill(pid, SIGTERM) == 0)
            {
                printf("Sent kill signal to job [%d]\n", pid);
                remove_job(target_str);
            }
            else
            {
                perror("Failed to kill the job");
            }
        }
        return 1;
    }
    return 0;
}

int handle_exit(char *input)
{
    if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
    {
        printf("Exiting Quash...\n");
        exit(0);
    }
    return 0;
}

void execute_command(char *cmd, int background)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        char *args[BUFFER_SIZE];
        char *input_file = NULL;
        char *output_file = NULL;
        int append_mode = 0;

        char *token = strtok(cmd, " ");
        int i = 0;
        while (token != NULL)
        {
            if (token[0] == '"' || token[0] == '\'')
            {
                token++;
                token[strlen(token) - 1] = '\0';
            }
            if (strcmp(token, "<") == 0)
            {
                input_file = strtok(NULL, " ");
            }
            else if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0)
            {
                append_mode = (strcmp(token, ">>") == 0);
                output_file = strtok(NULL, " ");
            }
            else
            {
                args[i++] = token;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (input_file)
        {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
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
        // perror("Execution failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        if (background)
            add_job(pid, cmd);
        else
            waitpid(pid, NULL, 0);
    }
    else
        perror("fork");
}