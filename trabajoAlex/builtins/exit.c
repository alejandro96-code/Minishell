#include "../minishell.h"

/*
Finaliza el programa con un código de salida.
Si se le pasa un número, lo usa como código.
*/

int builtin_exit(char **args)
{
    int status = 0;

    if (args[1])
        status = atoi(args[1]); // validación pendiente
    printf("exit\n");
    exit(status);
}