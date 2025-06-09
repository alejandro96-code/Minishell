#include "../minishell.h"

// Función para verificar si un string contiene un asterisco (*)
int contains_wildcard(const char *str)
{
    if (!str)
        return 0;
    
    return (ft_strchr(str, '*') != NULL);
}

// Función para verificar si un nombre de archivo coincide con un patrón con wildcard
int match_pattern(const char *pattern, const char *filename)
{
    if (!pattern || !filename)
        return 0;
    
    // Si no hay wildcard, comparar directamente
    if (!contains_wildcard(pattern))
        return (ft_strncmp(pattern, filename, ft_strlen(pattern)) == 0 && ft_strlen(pattern) == ft_strlen(filename));
    
    // Dividir el patrón por el wildcard
    char **parts = ft_split(pattern, '*');
    if (!parts)
        return 0;
    
    int match = 1;
    size_t filename_len = ft_strlen(filename);
    size_t pos = 0;
    int i = 0;
    
    // Si el patrón comienza con asterisco, no necesitamos verificar el inicio
    if (pattern[0] != '*' && parts[0] && ft_strlen(parts[0]) > 0)
    {
        // Verificar si el archivo comienza con la primera parte
        if (ft_strncmp(filename, parts[0], ft_strlen(parts[0])) != 0)
            match = 0;
        pos = ft_strlen(parts[0]);
    }
    
    // Verificar partes intermedias
    while (parts[i] && match)
    {
        // Omitir la primera parte si ya la verificamos
        if (i == 0 && pattern[0] != '*')
        {
            i++;
            continue;
        }
        
        // Si es una parte vacía, continuar
        if (ft_strlen(parts[i]) == 0)
        {
            i++;
            continue;
        }
        
        // Buscar la siguiente parte en el resto del nombre de archivo
        char *found = ft_strnstr(filename + pos, parts[i], ft_strlen(filename) - pos);
        if (!found)
        {
            match = 0;
            break;
        }
        
        // Actualizar la posición
        pos = (found - filename) + ft_strlen(parts[i]);
        i++;
    }
    
    // Si el patrón no termina con asterisco, verificar que el archivo termine con la última parte
    if (match && pattern[ft_strlen(pattern) - 1] != '*' && parts[i - 1] && ft_strlen(parts[i - 1]) > 0)
    {
        if (pos != filename_len)
            match = 0;
    }
    
    // Liberar memoria
    i = 0;
    while (parts[i])
        free(parts[i++]);
    free(parts);
    
    return match;
}

// Función para contar cuántos archivos coinciden con un patrón
int count_matching_files(const char *pattern)
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    
    dir = opendir(".");
    if (!dir)
        return 0;
    
    while ((entry = readdir(dir)) != NULL)
    {
        // Ignorar archivos ocultos a menos que el patrón comience con un punto
        if (entry->d_name[0] == '.' && pattern[0] != '.')
            continue;
        
        if (match_pattern(pattern, entry->d_name))
            count++;
    }
    
    closedir(dir);
    return count;
}

// Función para expandir un argumento con wildcard a múltiples argumentos
char **expand_wildcards(const char *arg, int *num_expanded)
{
    DIR *dir;
    struct dirent *entry;
    char **result = NULL;
    int count = 0;
    int capacity = 0;
    
    // Si no hay wildcard, devolver el argumento original
    if (!contains_wildcard(arg))
    {
        result = malloc(2 * sizeof(char *));
        if (!result)
            return NULL;
        
        result[0] = ft_strdup(arg);
        result[1] = NULL;
        *num_expanded = 1;
        return result;
    }
    
    // Contar cuántos archivos coinciden
    capacity = count_matching_files(arg);
    if (capacity == 0)
    {
        // Si no hay coincidencias, devolver el patrón original
        result = malloc(2 * sizeof(char *));
        if (!result)
            return NULL;
        
        result[0] = ft_strdup(arg);
        result[1] = NULL;
        *num_expanded = 1;
        return result;
    }
    
    // Asignar memoria para los resultados
    result = malloc((capacity + 1) * sizeof(char *));
    if (!result)
        return NULL;
    
    // Llenar el array con los archivos que coinciden
    dir = opendir(".");
    if (!dir)
    {
        free(result);
        return NULL;
    }
    
    while ((entry = readdir(dir)) != NULL && count < capacity)
    {
        // Ignorar archivos ocultos a menos que el patrón comience con un punto
        if (entry->d_name[0] == '.' && arg[0] != '.')
            continue;
        
        if (match_pattern(arg, entry->d_name))
        {
            result[count] = ft_strdup(entry->d_name);
            count++;
        }
    }
    
    result[count] = NULL;
    *num_expanded = count;
    
    closedir(dir);
    return result;
}

// Función principal para expandir wildcards en un array de argumentos
char **expand_wildcards_in_args(char **args, int *num_args)
{
    char **new_args;
    int total_expanded;
    int i;
    int j;
    int has_wildcards;

    if (!args || !*args)
        return (args);
    
    // Verificar si hay wildcards
    has_wildcards = 0;
    i = 0;
    while (args[i] != NULL)
    {
        if (contains_wildcard(args[i]))
        {
            has_wildcards = 1;
            break ;
        }
        i++;
    }

    // Si no hay wildcards, no hacer nada
    if (!has_wildcards)
        return (args);

    new_args = malloc(1024 * sizeof(char *)); // Tamaño inicial grande
    if (!new_args)
        return (args);

    total_expanded = 0;
    i = 0;
    while (args[i] != NULL)
    {
        if (contains_wildcard(args[i]))
        {
            int num_expanded = 0;
            char **expanded = expand_wildcards(args[i], &num_expanded);

            if (expanded)
            {
                j = 0;
                while (j < num_expanded)
                {
                    new_args[total_expanded++] = expanded[j];
                    j++;
                }
                free(expanded); // Liberar el array, pero no su contenido
            }
            else
            {
                new_args[total_expanded++] = ft_strdup(args[i]);
            }
        }
        else
        {
            new_args[total_expanded++] = ft_strdup(args[i]);
        }
        i++;
    }

    new_args[total_expanded] = NULL;
    *num_args = total_expanded;

    i = 0;
    while (args[i] != NULL)
    {
        free(args[i]);
        i++;
    }
    free(args);
    return (new_args);
}