
#include "../minishell.h"

//Funcion que mantiene el bucle y delega la expansión
char *expand_variable(char *str, char **env)
{
    if (!str || !env)
        return strdup("");

    char *result = malloc(4096);
    if (!result)
        return strdup(str);

    int i = 0;
    int j = 0;
    while (str[i] && j < 4095)
    {
        if (str[i] == '$')
        {
            // Ignorar $? o reemplazarlo con "0"
            if (str[i + 1] == '?')
            {
                result[j++] = '0';
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
    if (!str || !result || !j || !env)
        return 0;
        
    char var_name[256] = {0};
    int name_len = 0;

    // Extraer el nombre de la variable
    while (str[name_len] && (isalnum(str[name_len]) || str[name_len] == '_') && name_len < 255)
    {
        var_name[name_len] = str[name_len];
        name_len++;
    }
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
    }
    return name_len;
}