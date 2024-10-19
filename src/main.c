#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void run_quash();
void handle_exit(char *input);
void run_echo(char *input); // Declare run_echo here
void export(char *input);   // Declare builtin_export here
void pwd();                 // Declare builtin_export here

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

        // If the input starts with "echo", call the run_echo() function
        if (strncmp(buffer, "echo", 4) == 0)
        {
            run_echo(buffer);
        }
        // If the input starts with "export", call the builtin_export() function
        else if (strncmp(buffer, "export", 6) == 0)
        {
            export(buffer);
        }
        else if (strncmp(buffer, "pwd", 8) == 0)
        {
            pwd();
        }

        // Handle unknown commands without printing "You entered"
        else
        {
            printf("Command not recognized: %s\n", buffer);
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