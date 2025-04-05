#include "../minishell.h"
/*
    Cambia el directorio actual.
    Si no recibe argumento, va al HOME.
*/

int builtin_cd(char **args, char **env)
{
    (void)env;  // Si no planeas usar env, está bien, de lo contrario, podrías usarlo

    char *path = args[1];

    // Si no se pasa ningún argumento, se va al directorio HOME
    if (!path)
        path = getenv("HOME");

    // Si no se encuentra HOME o chdir falla, se muestra un error
    if (!path)
    {
        fprintf(stderr, "cd: HOME not set\n");
        return (1);
    }

    if (chdir(path) != 0)
    {
        perror("cd");
        return (1);
    }

    return (0);
}