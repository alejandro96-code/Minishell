#include "../minishell.h"

/*
    Elimina una variable del entorno si existe.
*/

int builtin_unset(char **args, char ***env)
{
    int i = 1;
    int j;
    int env_size = 0;
    char *var_to_remove;

    // Verificar que se pase al menos un argumento a unset
    if (!args[1])
    {
        fprintf(stderr, "unset: not enough arguments\n");
        return (1);
    }

    // Calcular el tamaño del array de entorno
    while ((*env)[env_size])
        env_size++;

    printf("Tamaño inicial del entorno: %d\n", env_size); // Depuración: tamaño inicial del entorno

    // Recorrer las variables a eliminar
    while (args[i])
    {
        var_to_remove = args[i];
        printf("Tratando de eliminar la variable: %s\n", var_to_remove); // Depuración: variable a eliminar

        // Buscar la variable en el entorno y eliminarla
        j = 0;
        while (j < env_size)
        {
            // Compara el nombre de la variable y verifica el '=' al final
            if (strncmp((*env)[j], var_to_remove, strlen(var_to_remove)) == 0 && (*env)[j][strlen(var_to_remove)] == '=')
            {
                printf("Variable encontrada. Eliminando: %s\n", (*env)[j]); // Depuración: variable encontrada
                free((*env)[j]); // Liberar la memoria de la variable

                // Desplazar el array de entorno hacia atrás para llenar el hueco
                while (j < env_size - 1)
                {
                    (*env)[j] = (*env)[j + 1];
                    j++;
                }

                (*env)[env_size - 1] = NULL; // Establecer el último valor a NULL
                env_size--;  // Reducir el tamaño del array
                break; // Salir del bucle de búsqueda una vez eliminada la variable
            }
            j++;
        }
        i++;
    }

    // Depuración: mostrar el entorno final
    printf("Entorno final después de unset:\n");
    i = 0;
    while ((*env)[i]) // Usando `while` para recorrer el entorno
    {
        printf("%s\n", (*env)[i]);
        i++;
    }

    return (0);
}
