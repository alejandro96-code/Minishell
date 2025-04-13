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

// Implementación del builtin cd
// Implementación del builtin cd con mejor gestión de memoria
int builtin_cd(char **args, char **env)
{
    char *path = NULL;
    char *home = NULL;
    char *new_path = NULL;
    char cwd[1024];
    int result = 0;

    if (!args || !env)
        return 1;

    path = args[1];

    // Si no se pasa ningún argumento, se va al directorio HOME
    if (!path)
    {
        home = get_env_var("HOME", env);
        if (!home)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
        path = home;
    }

    // Si la ruta es relativa, se resuelve en relación al directorio actual
    if (path[0] != '/' && path[0] != '~')
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            // Asegurarse de que hay espacio suficiente
            new_path = safe_malloc(strlen(cwd) + strlen(path) + 2); // +2 para '/' y '\0'
            if (!new_path)
                return 1;
            
            strcpy(new_path, cwd);
            strcat(new_path, "/");
            strcat(new_path, path);
            
            result = chdir(new_path);
            safe_free((void **)&new_path);
            
            if (result != 0)
            {
                perror("cd");
                return 1;
            }
            return 0;
        }
        else
        {
            perror("cd");
            return 1;
        }
    }
    // Si la ruta empieza con '~', es una ruta relativa al directorio HOME
    else if (path[0] == '~')
    {
        home = get_env_var("HOME", env);
        if (home)
        {
            // Reemplazar el '~' por el directorio HOME
            new_path = safe_malloc(strlen(home) + strlen(path)); // No +1 porque ya quitamos el '~'
            if (!new_path)
                return 1;
            
            strcpy(new_path, home);
            strcat(new_path, path + 1);  // Eliminar '~' al concatenar
            
            // Usar new_path y luego liberarlo
            result = chdir(new_path);
            safe_free((void **)&new_path);
            
            if (result != 0)
            {
                perror("cd");
                return 1;
            }
            return 0;
        }
        else
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }

    // Si llegamos aquí, path es una ruta absoluta
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }

    return 0;
}