#include "../minishell.h"

char *expand_variable(char *str, char **env)
{
    if (!str)
        return NULL;
    
    // Buffer para construir el resultado
    char *result = malloc(4096);
    if (!result)
        return strdup(str);
    
    int i = 0;  // Índice para str
    int j = 0;  // Índice para result
    
    while (str[i] && j < 4095)
    {
        if (str[i] == '$' && str[i+1] != '\0' && str[i+1] != ' ')
        {
            i++;  // Avanzar después del $
            
            // Extraer el nombre de la variable
            char var_name[256] = {0};
            int name_len = 0;
            
            while (str[i] && (isalnum(str[i]) || str[i] == '_') && name_len < 255)
            {
                var_name[name_len++] = str[i++];
            }
            var_name[name_len] = '\0';
            
            if (name_len > 0)
            {
                int k = 0;
                int found = 0;  // Se mantiene para verificar si se encontró la variable
                
                while (env[k])
                {
                    // Comprobar si comienza con "VAR="
                    if (strncmp(env[k], var_name, name_len) == 0 && env[k][name_len] == '=')
                    {
                        // Obtener el valor (después del =)
                        char *value = env[k] + name_len + 1;
                        
                        // Copiar el valor al resultado
                        int value_len = strlen(value);
                        if (j + value_len < 4095)
                        {
                            strcpy(result + j, value);
                            j += value_len;
                        }
                        
                        found = 1;  // Marcamos como encontrado
                        break;
                    }
                    k++;
                }

                // Si la variable no se encontró, podrías hacer algo, como imprimir un mensaje de advertencia:
                if (!found)
                {
                    fprintf(stderr, "Warning: Variable %s not found in environment\n", var_name);
                }
            }
        }
        else
        {
            // Simplemente copiar el carácter actual
            result[j++] = str[i++];
        }
    }
    
    // Asegurar que el resultado esté terminado correctamente
    result[j] = '\0';
    
    return result;
}
