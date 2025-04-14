#include "../minishell.h"

// Función para obtener el valor de una variable de entorno
char *get_env_var(char *name, char **env)
{
    int cont = 0;
    size_t name_len = strlen(name);
    
    while (env[cont])
    {
        if (ft_strncmp(env[cont], name, name_len) == 0 && env[cont][name_len] == '=')
            return &env[cont][name_len + 1];
        cont++;
    }
    return NULL;
}

// Implementación del builtin cd con mejor gestión de memoria
int builtin_cd(char **args, char **env)
{
    char *path = NULL;
    char *home = NULL;

    // Verificar argumentos
    if (!args || !env)
        return 1;

    path = args[1];

    // Si no se pasa ningún argumento, ir al HOME
    if (!path)
    {
        home = get_env_var("HOME", env);
        if (!home)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
        return chdir(home); // Simplificado, no necesita copiar home
    }

    // Si la ruta empieza con ~, es relativa al HOME
    if (path[0] == '~')
    {
        home = get_env_var("HOME", env);
        if (!home)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
        
        // Crear la ruta completa
        char *full_path = malloc(strlen(home) + strlen(path));
        if (!full_path)
            return 1;
        
        strcpy(full_path, home);
        strcat(full_path, path + 1);
        
        int result = chdir(full_path);
        free(full_path);
        
        if (result != 0)
        {
            perror("cd");
            return 1;
        }
        return 0;
    }

    // Caso directo con path absoluto o relativo simple
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }

    return 0;
}