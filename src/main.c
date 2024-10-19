#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

// Function declarations
void run_quash();
void handle_exit(char *input);
<<<<<<< HEAD
void run_echo(char *input); // Declare run_echo here
void export(char *input);   // Declare builtin_export here
void pwd();                 // Declare pwd function here
void cd(char *path);        // Declare cd function here
=======
void run_echo(char *input);       // Declare run_echo here
void builtin_export(char *input); // Declare builtin_export here
void pwd();                       // Declare builtin_export here
>>>>>>> 3994a8fb39a107b1ea7c8154fcde860e696aa677

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
<<<<<<< HEAD
            export(command);
        }
        // If the input starts with "pwd", call the pwd() function
        else if (strncmp(command, "pwd", 3) == 0)  // Check only the first 3 characters
=======
            builtin_export(buffer);
        }
        else if (strncmp(buffer, "pwd", 3) == 0)
>>>>>>> 3994a8fb39a107b1ea7c8154fcde860e696aa677
        {
            pwd();
        }
        // If the input starts with "cd", call the cd() function
        else if (strncmp(command, "cd", 2) == 0)  // Check only the first 2 characters
        {
            char *path = command + 3;  // Get the path (skip "cd ")
            cd(path); // Call the cd function
        }
        // Handle unknown commands without printing "You entered"
        else
        {
            printf("Command not recognized: %s\n", command);
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
