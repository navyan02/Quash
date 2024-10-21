#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "pipes.h"

void execute_pipe(char **cmd1, char **cmd2)
{
    int pipefd[2];
    pid_t cpid1, cpid2;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid1 = fork();
    if (cpid1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid1 == 0)
    {                                   // First child process
        close(pipefd[0]);               // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]);               // Close the write end after redirecting

        printf("Executing command 1: %s\n", cmd1[0]);
        execvp(cmd1[0], cmd1);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    {
        cpid2 = fork();
        if (cpid2 == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid2 == 0)
        {                                  // Second child process
            close(pipefd[1]);              // Close unused write end
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe
            close(pipefd[0]);              // Close the read end after redirecting

            printf("Executing command 2: %s\n", cmd2[0]);
            execvp(cmd2[0], cmd2);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {                     // Parent process
            close(pipefd[0]); // Close both ends of the pipe
            close(pipefd[1]);

            // Wait for both child processes to finish
            int status;
            waitpid(cpid1, &status, 0);
            waitpid(cpid2, &status, 0);
        }
    }
}
void execute_pipes(char ***cmds)
{
    int i = 0;
    int pipefd[2];
    int in_fd = 0; // The input file descriptor for the first command

    while (cmds[i] != NULL)
    {
        pipe(pipefd);

        if (fork() == 0)
        {
            dup2(in_fd, 0); // Redirect input from the previous command
            if (cmds[i + 1] != NULL)
            {
                dup2(pipefd[1], 1); // Redirect output to the next command
            }
            close(pipefd[0]);
            execvp(cmds[i][0], cmds[i]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            wait(NULL); // Wait for the child process to finish
            close(pipefd[1]);
            in_fd = pipefd[0]; // Save the input for the next command
            i++;
        }
    }
}
