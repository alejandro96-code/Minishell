#include "minishell.h"

/*
    Agrega o reemplaza una variable de entorno.
    Solo se permite si el formato es válido (VAR=VAL).
*/

int is_valid_export(char *str)
{
    return (str && strchr(str, '='));
}

int builtin_export(char **args, char ***env)
{
    int cont_args = 1;

    while (args[cont_args])
    {
        if (is_valid_export(args[cont_args]))
        {
            char *key = strtok(strdup(args[cont_args]), "=");
            int cont_env = 0;
            int replaced = 0;

            while ((*env)[cont_env])
            {
                if (strncmp((*env)[cont_env], key, strlen(key)) == 0 &&
                    (*env)[cont_env][strlen(key)] == '=')
                {
                    free((*env)[cont_env]);
                    (*env)[cont_env] = strdup(args[cont_args]);
                    replaced = 1;
                    break;
                }
                cont_env++;
            }
            if (!replaced)
            {
                int len = 0;
                while ((*env)[len])
                    len++;
                (*env)[len] = strdup(args[cont_args]);
                (*env)[len + 1] = NULL;
            }
            free(key);
        }
        else
            fprintf(stderr, "export: `%s': not a valid identifier\n", args[cont_args]);
        cont_args++;
    }
    return (0);
}