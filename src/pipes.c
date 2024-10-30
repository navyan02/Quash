#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "pipes.h"

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
