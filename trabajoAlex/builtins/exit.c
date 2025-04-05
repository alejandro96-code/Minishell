#include "../minishell.h"

/*
Finaliza el programa con un código de salida.
Si se le pasa un número, lo usa como código.
*/

int builtin_exit(char **args)
{
    int status = 0;

    // Si hay un argumento
    if (args[1])
    {
        // Convertir el argumento a número
        char *endptr;
        status = strtol(args[1], &endptr, 10);

        // Validar que el argumento sea un número
        if (*endptr != '\0')  // Si no es un número válido
        {
            fprintf(stderr, "exit: %s: numeric argument required\n", args[1]);
            return 1;  // Error, pero no salir
        }

        // Si hay más de un argumento
        if (args[2])
        {
            fprintf(stderr, "exit: too many arguments\n");
            return 1;
        }
    }

    // Si no hay errores, imprimir "exit" y salir con el código de estado
    printf("exit\n");
    exit(status);  // Termina el programa con el código de estado
}