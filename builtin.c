#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
