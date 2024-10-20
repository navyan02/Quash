#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// Function declarations
void run_quash();
void handle_exit(char *input);
void run_echo(char *input);          // Declare run_echo here
void cd(char *path);                 // Declare cd function here
void builtin_export(char *input);    // Declare builtin_export here
void pwd();                           // Declare pwd function here
void execute_command(char *cmd, int background); // Declare execute_command here
void handle_jobs_command();           // Declare jobs handling function

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

        // Check for background command
        int background = (command[strlen(command) - 1] == '&');
        if (background) {
            command[strlen(command) - 1] = '\0';  // Remove the '&' symbol
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
        else if (strncmp(command, "pwd", 3) == 0)
        {
            pwd();
        }
        // If the input starts with "cd", call the cd() function
        else if (strncmp(command, "cd", 2) == 0)  // Check only the first 2 characters
        {
            char *path = command + 3;  // Get the path (skip "cd ")
            cd(path); // Call the cd function
        }
        // Handle the "jobs" command
        else if (strcmp(command, "jobs") == 0)
        {
            handle_jobs_command();
        }
        // Handle other unknown commands
        else
        {
            execute_command(command, background);
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
