#include <stdio.h>
#include <stdlib.h> // Include this for getenv()
#include <string.h>
#include <unistd.h> // for getcwd
#include <limits.h> // for PATH_MAX
#include <wordexp.h>

void run_echo(char *input)
{
    // Remove "echo " part and get the rest
    char *message = input + 5; // Skip the "echo " part

    // Check if the message starts and ends with a single quote
    size_t len = strlen(message);
    if (len > 1 && message[0] == '\'' && message[len - 1] == '\'')
    {
        // Strip the single quotes
        message[len - 1] = '\0'; // Remove closing quote
        message++;               // Move pointer to skip opening quote
    }

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

void builtin_export(char *input)
{
    // Skip the "export " part
    char *var = input + 7;

    // Find the '=' character
    char *equal_sign = strchr(var, '=');
    if (equal_sign == NULL)
    {
        fprintf(stderr, "export: invalid format\n");
        return;
    }

    // Split the variable name and value
    *equal_sign = '\0';
    char *name = var;
    char *value = equal_sign + 1;

    // Use wordexp to expand environment variables
    wordexp_t p;
    if (wordexp(value, &p, 0) != 0)
    {
        fprintf(stderr, "export: wordexp failed\n");
        return;
    }

    // Use the expanded value
    char *expanded_value = p.we_wordv[0];

    // Debug prints
    // printf("Setting environment variable: %s=%s\n", name, expanded_value);

    // Set the environment variable
    if (setenv(name, expanded_value, 1) != 0)
    {
        perror("setenv");
    }

    // Free the memory allocated by wordexp
    wordfree(&p);
}

void pwd()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("getcwd");
    }
}