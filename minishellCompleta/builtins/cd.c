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
int builtin_cd(char **args, char **env)
{
    char *path = args[1];

    // Si no se pasa ningún argumento, se va al directorio HOME
    if (!path)
    {
        path = get_env_var("HOME", env);
        if (!path)
        {
            perror("cd: HOME not set\n");
            return 1;
        }
    }

    // Si la ruta es relativa, se resuelve en relación al directorio actual
    if (path[0] != '/' && path[0] != '~')
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            // Concatenar la ruta relativa al directorio actual
            ft_strcat(cwd, "/");
            ft_strcat(cwd, path);
            path = cwd;  // Ahora 'path' es la ruta absoluta
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
        char *home = get_env_var("HOME", env);
        if (home)
        {
            // Reemplazar el '~' por el directorio HOME
            char *new_path = malloc(strlen(home) + strlen(path));
            if (!new_path)
            {
                perror("malloc");
                return 1;
            }
            strcpy(new_path, home);
            ft_strcat(new_path, path + 1);  // Eliminar '~' al concatenar
            path = new_path;
        }
        else
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }

    // Intentar cambiar al directorio
    if (chdir(path) != 0)
    {
        // En caso de error, mostrar el error específico
        perror("cd");
        return 1;
    }

    return 0;
}
