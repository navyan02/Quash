#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void run_quash();
void handle_exit(char *input);

int main()
{
    printf("Welcome to Quash!\n");
    run_quash();
    return 0;
}

// main loop for Quash
void run_quash()
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("[QUASH]$ "); // shell prompt
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            perror("Error reading input");
            continue;
        }

        // strip newline character from input
        buffer[strcspn(buffer, "\n")] = 0;

        // handle the "exit" or "quit" commands
        handle_exit(buffer);

        // placeholder for future command handling logic
        printf("You entered: %s\n", buffer);
    }
}

// handle exit or quit command
void handle_exit(char *input)
{
    if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
    {
        printf("Exiting Quash...\n");
        exit(0);
    }
}
