#include "../minishell.h"

/*
Imprime en pantalla lo que se le pase como argumento.
Si la opción -n está presente,
no imprime el salto de línea final.
*/

int builtin_echo(char **args, char **env)
{
    int cont = 1;
    int print_newline = 1;
    
    // Comprobar si hay opción -n
    if (args[1] && strcmp(args[1], "-n") == 0)
    {
        print_newline = 0;
        cont = 2;  // Comenzar desde el siguiente argumento
    }

    while (args[cont])
    {
        // Actualizar para usar la nueva firma de expand_variable
        char *expanded_str = expand_variable(args[cont], env);
        printf("%s", expanded_str);
        free(expanded_str);

        if (args[cont + 1])  // Si hay más argumentos, poner un espacio
            printf(" ");
        cont++;
    }

    if (print_newline)
        printf("\n");
        
    return (0);
}