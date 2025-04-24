#include "../minishell.h"

//Funcion que mantiene el bucle y delega la expansión
char *expand_variable(char *str, char **env, int last_exit_status)
{
    (void)last_exit_status;
    if (!str)
        return NULL;

    char *result = malloc(4096);
    if (!result)
        return strdup(str);

    int i = 0;
    int j = 0;
    while (str[i] && j < 4095)
    {
        if (str[i] == '$')
        {
            // Expandir $?
            if (str[i + 1] == '?')
            {
                char status_str[12];
                sprintf(status_str, "%d", last_exit_status);
                int status_len = strlen(status_str);
                
                if (j + status_len < 4095)
                {
                    strcpy(result + j, status_str);
                    j += status_len;
                }
                i += 2; // Saltar '$?'
            }
            else if (str[i + 1] != '\0' && str[i + 1] != ' ')
            {
                int consumed = process_env_variable(str + i + 1, result, &j, env);
                i += consumed + 1;  // +1 por el '$'
            }
            else
            {
                result[j++] = str[i++];
            }
        }
        else
        {
            result[j++] = str[i++];
        }
    }
    result[j] = '\0';
    return result;
}

//Funcion que process_env_variable: se encarga de procesar la expansión de $VAR
int process_env_variable(char *str, char *result, int *j, char **env)
{
    char var_name[256] = {0};
    int name_len = 0;

    while (str[name_len] && (isalnum(str[name_len]) || str[name_len] == '_') && name_len < 255)
        var_name[name_len] = str[name_len], name_len++;

    var_name[name_len] = '\0';

    if (name_len > 0)
    {
        int k = 0;
        while (env[k])
        {
            if (strncmp(env[k], var_name, name_len) == 0 && env[k][name_len] == '=')
            {
                char *value = env[k] + name_len + 1;
                int value_len = strlen(value);
                if (*j + value_len < 4095)
                {
                    strcpy(result + *j, value);
                    *j += value_len;
                }
                return name_len;
            }
            k++;
        }
        fprintf(stderr, "Warning: Variable %s not found in environment\n", var_name);
    }
    return name_len;
}