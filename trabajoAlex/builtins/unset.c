#include "minishell.h"

/*
    Elimina una variable del entorno si existe.
*/
int builtin_unset(char **args, char ***env)
{
    int cont_args = 1;

    while (args[cont_args])
    {
        int cont_env = 0;
        while ((*env)[cont_env])
        {
            if (strncmp((*env)[cont_env], args[cont_args], strlen(args[cont_args])) == 0 &&
                (*env)[cont_env][strlen(args[cont_args])] == '=')
            {
                free((*env)[cont_env]);
                while ((*env)[cont_env])
                {
                    (*env)[cont_env] = (*env)[cont_env + 1];
                    cont_env++;
                }
                continue;
            }
            cont_env++;
        }
        cont_args++;
    }
    return (0);
}
