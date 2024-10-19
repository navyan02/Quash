#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void run_quash();
void handle_exit(char *input);
void run_echo(char *input);  // Declare run_echo here

int main()
{
    printf("Welcome to Quash!\n");
    run_quash();
    return 0;
}

// main loop for Quash
void run_quash()
{
    char input_buffer[1024];
    char *args[64]; // To store parsed arguments
    int num_args;   // Number of arguments

    while (1)
    {
        printf("[QUASH]$ ");
        fgets(input_buffer, 1024, stdin); // Read user input

        // Remove the newline character from the input
        input_buffer[strcspn(input_buffer, "\n")] = 0;

        // Tokenize the input into arguments
        num_args = 0;
        args[num_args] = strtok(input_buffer, " ");
        while (args[num_args] != NULL)
        {
            num_args++;
            args[num_args] = strtok(NULL, " ");
        }

        if (num_args == 0)
        {
            continue; // No command entered, go to the next iteration
        }

        // Check for built-in commands
        if (strcmp(args[0], "pwd") == 0)
        {
            builtin_pwd();
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            builtin_echo(args, num_args);
        }
        else if (strcmp(args[0], "export") == 0)
        {
            builtin_export(args, num_args);
        }
        else if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0)
        {
            break; // Exit the shell
        }
        else
        {
            // Handle external commands (to be implemented)
            printf("Command not found: %s\n", args[0]);
        }
    }
}