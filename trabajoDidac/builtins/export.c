#include "../minishell.h"

/*
    Agrega o reemplaza una variable de entorno.
    Solo se permite si el formato es válido (VAR=VAL).
*/

// Valida que el formato sea VAR=VAL
int is_valid_export(char *str)
{
    int i = 0;
    
    // Verificar que el primer carácter sea válido para un nombre de variable
    if (!str || !str[0] || !(isalpha(str[0]) || str[0] == '_'))
        return 0;
        
    // Verificar que el nombre de la variable solo contenga caracteres válidos
    while (str[i] && str[i] != '=')
    {
        if (!(isalnum(str[i]) || str[i] == '_'))
            return 0;
        i++;
    }
    
    // Verificar que contenga un '='
    return (str[i] == '=');
}

int builtin_export(char **args, char ***env)
{
    int cont_args = 1;

    while (args[cont_args])
    {
        if (is_valid_export(args[cont_args]))
        {
            // Obtener la clave sin modificar el argumento original
            char *arg_copy = strdup(args[cont_args]);
            char *equal_sign = strchr(arg_copy, '=');
            *equal_sign = '\0';  // Temporalmente dividir la cadena
            char *key = arg_copy;
            
            int cont_env = 0;
            int replaced = 0;

            // Buscamos si la clave ya existe en el entorno
            while ((*env)[cont_env])
            {
                char *env_equal = strchr((*env)[cont_env], '=');
                if (env_equal && (env_equal - (*env)[cont_env]) == (long)strlen(key) &&  // Conversión explícita de tamaño
                    strncmp((*env)[cont_env], key, strlen(key)) == 0)
                {
                    free((*env)[cont_env]);
                    (*env)[cont_env] = strdup(args[cont_args]);
                    replaced = 1;
                    break;
                }
                cont_env++;
            }

            // Si no encontramos la variable, la agregamos
            if (!replaced)
            {
                int len = 0;
                while ((*env)[len]) {
                    len++;
                }

                // Reasignamos espacio para agregar la nueva variable
                char **new_env = realloc(*env, sizeof(char *) * (len + 2));
                if (!new_env) {
                    perror("realloc");
                    free(arg_copy);
                    return 1;
                }
                *env = new_env;

                (*env)[len] = strdup(args[cont_args]);
                (*env)[len + 1] = NULL;
            }

            // Liberamos la memoria de la copia
            free(arg_copy);
        }
        else
        {
            fprintf(stderr, "export: `%s': not a valid identifier\n", args[cont_args]);
        }
        cont_args++;
    }

    return (0);
}