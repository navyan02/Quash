#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for getcwd
#include <limits.h> // for PATH_MAX
#include <wordexp.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

void run_echo(char *input)
{
    // Remove "echo " part and get the rest
    char *message = input + 5; // Skip the "echo " part

    // Remove leading spaces
    while (*message == ' ')
    {
        message++;
    }

    // Use an integer to track quote state (0 = outside quotes, positive = inside quotes)
    int quote_state = 0;

    // Create a buffer to store the final output
    char output[BUFFER_SIZE]; // Use a defined buffer size
    int output_index = 0;

    // Iterate through each character in the message
    for (size_t i = 0; i < strlen(message); i++)
    {
        char current_char = message[i];

        // Toggle quote state for single quotes
        if (current_char == '\'')
        {
            quote_state++;
            continue; // Skip the quote character
        }

        // Toggle quote state for double quotes
        if (current_char == '\"')
        {
            quote_state++;
            continue; // Skip the quote character
        }

        // Handle environment variables
        if (current_char == '$' && (quote_state % 2 == 0))
        {
            // Get the name of the environment variable
            size_t var_start = i + 1;
            size_t var_length = 0;

            // Determine the length of the variable name (stop at non-alphanumeric or underscore)
            while (isalnum(message[var_start + var_length]) || message[var_start + var_length] == '_')
            {
                var_length++;
            }

            // Extract the variable name
            char var_name[64]; // Assume variable names won't exceed 63 characters
            strncpy(var_name, message + var_start, var_length);
            var_name[var_length] = '\0'; // Null-terminate the variable name

            // Get the value of the environment variable
            char *var_value = getenv(var_name);
            if (var_value != NULL)
            {
                // Add the environment variable value to output
                size_t value_length = strlen(var_value);
                if (output_index + value_length < BUFFER_SIZE)
                {
                    strncpy(&output[output_index], var_value, value_length);
                    output_index += value_length;
                }
            }
            else
            {
                // Print an error message if the variable is not found (for debugging)
                fprintf(stderr, "Warning: Environment variable %s not found.\n", var_name);
            }

            // Move the index past the variable name
            i += var_length; // Skip the variable name
            continue;        // Skip the `$` character
        }

        // If we encounter a space and we're not in quotes, treat it as a delimiter
        if (current_char == ' ' && (quote_state % 2 == 0))
        {
            // Only add a space to output if the last character is not already a space
            if (output_index > 0 && output[output_index - 1] != ' ')
            {
                output[output_index++] = ' ';
            }
            continue; // Skip adding the space character
        }

        // Add current character to output
        output[output_index++] = current_char;
    }

    // Null-terminate the output string
    output[output_index] = '\0';

    // Print the final message
    printf("%s\n", output);
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

void cd(char *path)
{
    char cwd[BUFFER_SIZE];       // Buffer to hold the current working directory
    char *home = getenv("HOME"); // Get the HOME environment variable

    // If no path is provided, change to the HOME directory
    if (path == NULL || strlen(path) == 0)
    {
        path = home;
    }

    // Handle paths that start with "$"
    if (path[0] == '$')
    {
        // Get the environment variable name (e.g., "HOME")
        char *env_var_name = path + 1;              // Skip the '$'
        char *env_var_value = getenv(env_var_name); // Get the environment variable value

        if (env_var_value)
        {
            path = env_var_value; // Use the expanded environment variable value as the path
        }
        else
        {
            fprintf(stderr, "cd: No such environment variable: %s\n", env_var_name);
            return;
        }
    }

    // Change the current working directory
    if (chdir(path) == -1)
    {
        perror("cd error"); // Print error if chdir fails
        return;
    }

    // Update the PWD environment variable
    getcwd(cwd, sizeof(cwd)); // Get the new current working directory
    setenv("PWD", cwd, 1);    // Update PWD environment variable
}