#include "minishell.h"

/*
    Imprime todas las variables del entorno en formato KEY=VALUE.
*/

int builtin_env(char **env)
{
    int cont = 0;

    while (env[cont])
    {
        printf("%s\n", env[cont]);
        cont++;
    }
    return (0);
}