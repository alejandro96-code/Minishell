#include "../minishell.h"
#include "../libft/libft.h"

char *expand_variable(char *str, char **env)
{
    char *expanded_str = strdup(str);
    if (!expanded_str)
        return NULL;

    int i = 0;
    while (env[i])
    {
        char *key = strtok(strdup(env[i]), "="); // Obtener la clave
        char *value = strchr(env[i], '=') + 1;   // Obtener el valor

        // Buscar la variable en la cadena
        if (key && value)
        {
            char *pos = strstr(expanded_str, "$"); // Buscar el signo $ indicando una variable
            if (pos)
            {
                if (strncmp(pos + 1, key, strlen(key)) == 0)
                {
                    size_t key_len = strlen(key);
                    size_t value_len = strlen(value);
                    char *new_str = malloc(strlen(expanded_str) - key_len + value_len + 1);

                    strncpy(new_str, expanded_str, pos - expanded_str);
                    strcpy(new_str + (pos - expanded_str), value);
                    strcpy(new_str + (pos - expanded_str) + value_len, pos + key_len + 1);

                    free(expanded_str);
                    expanded_str = new_str;
                }
            }
        }
        i++;
    }

    return expanded_str;
}