#include "../minishell.h"

/*
Imprime en pantalla lo que se le pase como argumento.
Si la opción -n está presente,
no imprime el salto de línea final.
*/

int builtin_echo(char **args, char **env)  // Agrega env como argumento
{
    int i = 1;

    while (args[i])
    {
        char *expanded_str = expand_variable(args[i], env);  // Pasamos env a expand_variable
        printf("%s", expanded_str);
        free(expanded_str);

        if (args[i + 1])  // Si hay más argumentos, poner un espacio
            printf(" ");
        i++;
    }

    printf("\n");
    return (0);
}
