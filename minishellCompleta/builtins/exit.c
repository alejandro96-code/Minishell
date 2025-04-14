#include "../minishell.h"

/*
Finaliza el programa con un código de salida.
Si se le pasa un número, lo usa como código.
*/

static int is_numeric(char *str)
{
    if (!str)
        return 0;
    
    // Ignorar espacios iniciales
    while (*str && isspace(*str))
        str++;
    
    // Permitir un signo opcional
    if (*str == '+' || *str == '-')
        str++;
    
    // Debe haber al menos un dígito
    if (!*str)
        return 0;
    
    // Revisar que todos los caracteres sean dígitos
    while (*str)
    {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    
    return 1;
}

int builtin_exit(char **args)
{
    int status = 0;

    // Si hay un argumento
    if (args[1])
    {
        // Validar que el argumento sea un número
        if (!is_numeric(args[1]))
        {
            fprintf(stderr, "exit: %s: numeric argument required\n", args[1]);
            exit(255);  // Código de error estándar para error de sintaxis
        }

        // Convertir el argumento a número
        status = atoi(args[1]);

        // Si hay más de un argumento
        if (args[2])
        {
            fprintf(stderr, "exit: too many arguments\n");
            return 1;  // Error, pero no salir
        }
    }

    // Si no hay errores, imprimir "exit" y salir con el código de estado
    printf("exit\n");
    exit(status);  // Termina el programa con el código de estado
}