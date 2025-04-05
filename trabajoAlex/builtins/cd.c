#include "../minishell.h"
/*
    Cambia el directorio actual.
    Si no recibe argumento, va al HOME.
*/

char *get_env_var(char *name, char **env)
{
    int i = 0;
    size_t name_len = strlen(name);
    
    while (env[i])
    {
        if (strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
            return &env[i][name_len + 1];
        i++;
    }
    return NULL;
}

int builtin_cd(char **args, char **env)
{
    char *path = args[1];

    // Si no se pasa ningún argumento, se va al directorio HOME
    if (!path)
    {
        path = get_env_var("HOME", env);
        if (!path)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return (1);
        }
    }

    // Intentar cambiar al directorio
    if (chdir(path) != 0)
    {
        // En caso de error, mostrar el error específico
        perror("cd");
        return (1);
    }

    return (0);
}