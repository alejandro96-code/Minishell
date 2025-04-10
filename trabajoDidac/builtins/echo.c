#include "../minishell.h"

/*
Imprime en pantalla lo que se le pase como argumento.
Si la opción -n está presente,
no imprime el salto de línea final.
*/

int builtin_echo(char **args, char **env)
{
    int i = 1;
    int print_newline = 1;
    
    // Comprobar si hay opción -n
    if (args[1] && strcmp(args[1], "-n") == 0)
    {
        print_newline = 0;
        i = 2;  // Comenzar desde el siguiente argumento
    }

    while (args[i])
    {
        char *expanded_str = expand_variable(args[i], env);
        printf("%s", expanded_str);
        free(expanded_str);

        if (args[i + 1])  // Si hay más argumentos, poner un espacio
            printf(" ");
        i++;
    }

    if (print_newline)
        printf("\n");
        
    return (0);
}