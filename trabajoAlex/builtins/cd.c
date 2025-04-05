#include "minishell.h"
/*
    Cambia el directorio actual.
    Si no recibe argumento, va al HOME.
*/

int builtin_cd(char **args, char **env)
{
    char *path = args[1];

    if (!path)
        path = getenv("HOME");
    if (!path || chdir(path) != 0)
    {
        perror("cd");
        return (1);
    }
    return (0);
}