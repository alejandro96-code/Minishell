#include "../minishell.h"


//Elimina una variable del entorno si existe.


int builtin_unset(char **args, char ***env)
{
    int cont = 1;
    
    // Verificar que se pase al menos un argumento a unset
    if (!args[1])
    {
        fprintf(stderr, "unset: not enough arguments\n");
        return (1);
    }

    // Recorrer las variables a eliminar
    while (args[cont])
    {
        char *var_to_remove = args[cont];
        size_t var_len = strlen(var_to_remove);
        
        // Buscar la variable en el entorno y eliminarla
        int j = 0;
        while ((*env)[j])
        {
            // Compara el nombre de la variable y verifica el '=' al final
            if (strncmp((*env)[j], var_to_remove, var_len) == 0 && 
                (*env)[j][var_len] == '=')
            {
                free((*env)[j]); // Liberar la memoria de la variable

                // Desplazar el array de entorno hacia atrás para llenar el hueco
                int k = j;
                while ((*env)[k + 1])
                {
                    (*env)[k] = (*env)[k + 1];
                    k++;
                }
                (*env)[k] = NULL; // Establecer el último valor a NULL
                
                // No incrementamos j, ya que ahora hay una nueva variable en esta posición
                continue;
            }
            j++;
        }
        cont++;
    }

    return (0);
}
