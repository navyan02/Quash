#include <stdio.h>
#include <stdlib.h> // Include this for getenv()
#include <string.h>

void run_echo(char *input)
{
    // Remove "echo" part and print the rest
    char *message = input + 5; // Skip the "echo " part

    // Check if it's an environment variable
    if (message[0] == '$')
    {
        char *env_var = getenv(message + 1); // Get the environment variable, skip the '$'
        if (env_var != NULL)
        {
            printf("%s\n", env_var);
        }
        else
        {
            printf("Environment variable %s not found\n", message + 1);
        }
    }
    else
    {
        printf("%s\n", message); // Normal echo
    }
}
void export(char **args, int num_args)
{
    if (num_args != 2)
    {
        fprintf(stderr, "export: Invalid format. Use: export VAR=VALUE\n");
        return;
    }
    // Split the argument at the '='
    char *var = strtok(args[1], "=");
    char *value = strtok(NULL, "=");

    if (var == NULL || value == NULL)
    {
        fprintf(stderr, "export: Invalid format. Use: export VAR=VALUE\n");
        return;
    }

    // Set the environment variable using setenv
    if (setenv(var, value, 1) == -1)
    {
        perror("export");
    }
}
