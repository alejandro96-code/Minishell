#include "../minishell.h"

/*
    Agrega o reemplaza una variable de entorno.
    Solo se permite si el formato es válido (VAR=VAL).
*/

// Valida que el formato sea VAR=VAL
static int is_valid_export(char *str)
{
    int cont = 0;
    
    // Verificar que el primer carácter sea válido para un nombre de variable
    if (!str || !str[0] || !(isalpha(str[0]) || str[0] == '_'))
        return 0;
        
    // Verificar que el nombre de la variable solo contenga caracteres válidos
    while (str[cont] && str[cont] != '=')
    {
        if (!(isalnum(str[cont]) || str[cont] == '_'))
            return 0;
        cont++;
    }
    
    // Verificar que contenga un '='
    return (str[cont] == '=');
}

int builtin_export(char **args, char ***env)
{
    int cont_args = 1;

    // Verificar argumentos
    if (!args || !env || !*env)
        return 1;

    // Si no hay argumentos, mostrar el entorno (como en Bash)
    if (!args[1])
    {
        return builtin_env(*env);
    }

    while (args[cont_args])
    {
        if (is_valid_export(args[cont_args]))
        {
            // Obtener la clave sin modificar el argumento original
            char *arg_copy = strdup(args[cont_args]);
            if (!arg_copy)
                return 1;
                
            char *equal_sign = strchr(arg_copy, '=');
            if (!equal_sign) {
                free(arg_copy);
                cont_args++;
                continue;
            }
            
            *equal_sign = '\0';  // Temporalmente dividir la cadena
            char *key = arg_copy;
            
            int cont_env = 0;
            int replaced = 0;

            // Buscamos si la clave ya existe
            while ((*env)[cont_env])
            {
                char *env_equal = strchr((*env)[cont_env], '=');
                if (env_equal && 
                    (size_t)(env_equal - (*env)[cont_env]) == strlen(key) &&
                    strncmp((*env)[cont_env], key, strlen(key)) == 0)
                {
                    // Reemplazar la variable existente
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

                // Reasignar espacio para agregar la nueva variable
                char **new_env = realloc(*env, sizeof(char *) * (len + 2));
                if (!new_env) {
                    perror("realloc");
                    free(arg_copy);
                    return 1;
                }
                *env = new_env;

                (*env)[len] = strdup(args[cont_args]);
                if (!(*env)[len]) {
                    perror("strdup");
                    free(arg_copy);
                    return 1;
                }
                (*env)[len + 1] = NULL;
            }

            free(arg_copy);
        }
        else
        {
            fprintf(stderr, "export: `%s': not a valid identifier\n", args[cont_args]);
        }
        cont_args++;
    }

    return 0;
}