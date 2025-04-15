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
    // Liberar memoria previa si existe
    free_command_names();
    
    // Crear lista con los builtins primero
    command_names = malloc(sizeof(char *) * 100);  // Espacio para hasta 100 comandos
    if (!command_names)
        return;
    
    int i = 0;
    
    // Agregar los builtins
    command_names[i++] = ft_strdup("cd");
    command_names[i++] = ft_strdup("echo");
    command_names[i++] = ft_strdup("pwd");
    command_names[i++] = ft_strdup("export");
    command_names[i++] = ft_strdup("unset");
    command_names[i++] = ft_strdup("env");
    command_names[i++] = ft_strdup("exit");
    
    // Agregar comandos del PATH
    char *path_env = get_env_var("PATH", env);
    if (path_env)
    {
        char **paths = ft_split(path_env, ':');
        if (paths)
        {
            int j = 0;
            while (paths[j])
            {
                DIR *dir = opendir(paths[j]);
                if (dir)
                {
                    struct dirent *entry;
                    while ((entry = readdir(dir)) != NULL && i < 99)
                    {
                        // Solo agregamos archivos ejecutables
                        char full_path[1024];
                        snprintf(full_path, sizeof(full_path), "%s/%s", paths[j], entry->d_name);
                        
                        // Verificar si es un archivo ejecutable
                        if (access(full_path, X_OK) == 0)
                        {
                            // Verificar si ya existe en la lista
                            int exists = 0;
                            for (int k = 0; k < i; k++)
                            {
                                if (strcmp(command_names[k], entry->d_name) == 0)
                                {
                                    exists = 1;
                                    break;
                                }
                            }
                            
                            if (!exists)
                                command_names[i++] = ft_strdup(entry->d_name);
                        }
                    }
                    closedir(dir);
                }
                j++;
            }
            ft_free_split(paths);
        }
    }
    
    command_names[i] = NULL;  // Terminar la lista con NULL
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