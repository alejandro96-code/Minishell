#include <stdio.h>
#include <string.h>

/*
Imprime en pantalla lo que se le pase como argumento.
Si la opción -n está presente,
no imprime el salto de línea final.
*/

int builtin_echo(char **args)
{
    int cont = 1;
    int newline = 1;

    if (args[cont] && strcmp(args[cont], "-n") == 0)
    {
        newline = 0;
        cont++;
    }
    while (args[cont])
    {
        printf("%s", args[cont]);
        if (args[cont + 1])
            printf(" ");
        cont++;
    }
    if (newline)
        printf("\n");
}
