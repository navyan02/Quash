#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

void run_echo(char *input) {
    // Remove "echo " part and get the rest
    char *message = input + 5; // Skip the "echo " part

    // Remove leading spaces
    while (*message == ' ') {
        message++;
    }

    // Use an integer to track quote state (0 = outside quotes, positive = inside quotes)
    int quote_state = 0;

    // Create a buffer to store the final output
    char output[BUFFER_SIZE];  // Use a defined buffer size
    int output_index = 0;

    // Iterate through each character in the message
    for (size_t i = 0; i < strlen(message); i++) {
        char current_char = message[i];

        // Toggle quote state for single quotes
        if (current_char == '\'') {
            quote_state++;
            continue; // Skip the quote character
        }

        // Toggle quote state for double quotes
        if (current_char == '\"') {
            quote_state++;
            continue; // Skip the quote character
        }

        // Handle environment variables
        if (current_char == '$' && (quote_state % 2 == 0)) {
            // Get the name of the environment variable
            size_t var_start = i + 1;
            size_t var_length = 0;

            // Determine the length of the variable name (stop at non-alphanumeric or underscore)
            while (isalnum(message[var_start + var_length]) || message[var_start + var_length] == '_') {
                var_length++;
            }

            // Extract the variable name
            char var_name[64]; // Assume variable names won't exceed 63 characters
            strncpy(var_name, message + var_start, var_length);
            var_name[var_length] = '\0'; // Null-terminate the variable name

            // Get the value of the environment variable
            char *var_value = getenv(var_name);
            if (var_value != NULL) {
                // Add the environment variable value to output
                size_t value_length = strlen(var_value);
                if (output_index + value_length < BUFFER_SIZE) {
                    strncpy(&output[output_index], var_value, value_length);
                    output_index += value_length;
                }
            } else {
                // Print an error message if the variable is not found (for debugging)
                fprintf(stderr, "Warning: Environment variable %s not found.\n", var_name);
            }

            // Move the index past the variable name
            i += var_length; // Skip the variable name
            continue; // Skip the `$` character
        }

        // If we encounter a space and we're not in quotes, treat it as a delimiter
        if (current_char == ' ' && (quote_state % 2 == 0)) {
            // Only add a space to output if the last character is not already a space
            if (output_index > 0 && output[output_index - 1] != ' ') {
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

void export(char **args, int num_args)
{
    if (num_args != 2)
    {
        fprintf(stderr, "export: Invalid format. Use: export VAR=VALUE\n");
        return;
    }

    // Split the argument at the first '='
    char *var = strtok(args[1], "=");
    char *value = strtok(NULL, "");

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

void cd(char *path) {
    char cwd[BUFFER_SIZE];  // Buffer to hold the current working directory
    char *home = getenv("HOME"); // Get the HOME environment variable

    // Check if no path is provided
    if (path == NULL || strlen(path) == 0) {
        // If no path, change to the HOME directory
        path = home; 
    }

    // Change the current working directory
    if (chdir(path) == -1) {
        perror("cd error");  // Print error if chdir fails
        return;
    }

    // Update the PWD environment variable
    getcwd(cwd, sizeof(cwd));  // Get the new current working directory
    setenv("PWD", cwd, 1);  // Update PWD environment variable
}
