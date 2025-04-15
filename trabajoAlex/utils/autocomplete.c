#include "../minishell.h"

char **command_names = NULL;  // Lista global de comandos para autocompletar

// Función auxiliar para liberar la memoria de command_names
void free_command_names(void)
{
    if (command_names)
    {
        int i = 0;
        while (command_names[i])
        {
            free(command_names[i]);
            i++;
        }
        free(command_names);
        command_names = NULL;
    }
}

// Inicializa la lista de comandos para autocompletar
void initialize_command_names(char **env)
{
    int i = 0;
    int j = 0;
    int k;
    int exists;
    DIR *dir;
    struct dirent *entry;
    char *path_env;
    char **paths;
    char full_path[1024];

    free_command_names();

    command_names = malloc(sizeof(char *) * 100);
    if (!command_names)
        return;

    command_names[i++] = ft_strdup("cd");
    command_names[i++] = ft_strdup("echo");
    command_names[i++] = ft_strdup("pwd");
    command_names[i++] = ft_strdup("export");
    command_names[i++] = ft_strdup("unset");
    command_names[i++] = ft_strdup("env");
    command_names[i++] = ft_strdup("exit");

    path_env = get_env_var("PATH", env);
    if (path_env)
    {
        paths = ft_split(path_env, ':');
        if (paths)
        {
            while (paths[j])
            {
                dir = opendir(paths[j]);
                if (dir)
                {
                    entry = readdir(dir);
                    while (entry && i < 99)
                    {
                        snprintf(full_path, sizeof(full_path), "%s/%s", paths[j], entry->d_name);
                        if (access(full_path, X_OK) == 0)
                        {
                            exists = 0;
                            k = 0;
                            while (k < i)
                            {
                                if (strcmp(command_names[k], entry->d_name) == 0)
                                {
                                    exists = 1;
                                    break;
                                }
                                k++;
                            }
                            if (!exists)
                                command_names[i++] = ft_strdup(entry->d_name);
                        }
                        entry = readdir(dir);
                    }
                    closedir(dir);
                }
                j++;
            }
            ft_free_split(paths);
        }
    }
    command_names[i] = NULL;
}


// Función de generador para readline
char *command_generator(const char *text, int state)
{
    static int list_index, len;
    
    // Si es el primer llamado, inicializa el contador y la longitud
    if (!state)
    {
        list_index = 0;
        len = strlen(text);
    }
    
    // Devuelve el siguiente nombre que coincida
    while (command_names && command_names[list_index])
    {
        char *name = command_names[list_index];
        list_index++;
        
        if (strncmp(name, text, len) == 0)
            return ft_strdup(name);  // Devuelve una copia
    }
    
    // No hay más coincidencias
    return NULL;
}

// Función de completado para readline
char **command_completion(const char *text, int start, int end)
{
    (void)end;
    // Desactivar la compleción por defecto (archivos)
    rl_attempted_completion_over = 1;
    
    // Solo completar comandos si estamos al inicio de la línea
    if (start == 0)
        return rl_completion_matches(text, command_generator);
    
    // En otro caso no mostrar sugerencias
    return NULL;
}

// Función para configurar el autocompletado
void setup_autocomplete(char **env)
{
    // Inicializar la lista de comandos
    initialize_command_names(env);
    
    // Configurar la función de autocompletado de readline
    rl_attempted_completion_function = command_completion;
}