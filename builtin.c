#include <stdio.h>
#include <stdlib.h>  // Include this for getenv()
#include <string.h>

void run_echo(char *input) {
    // Remove "echo" part and print the rest
    char *message = input + 5;  // Skip the "echo " part

    // Check if it's an environment variable
    if (message[0] == '$') {
        char *env_var = getenv(message + 1);  // Get the environment variable, skip the '$'
        if (env_var != NULL) {
            printf("%s\n", env_var);
        } else {
            printf("Environment variable %s not found\n", message + 1);
        }
    } else {
        printf("%s\n", message);  // Normal echo
    }
}
