#include <stdio.h>

/*
    Imprime todas las variables del entorno en formato KEY=VALUE.
*/

#include <stdio.h>

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