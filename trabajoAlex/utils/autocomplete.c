#include "../minishell.h"
#include <sys/stat.h>  // Para stat y S_ISDIR
#include <limits.h>    // Para PATH_MAX

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

// Función de generador para completar comandos
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

// Función para concatenar path de forma segura
char *safe_path_join(const char *dir, const char *file)
{
    size_t dir_len = strlen(dir);
    size_t file_len = strlen(file);
    char *result;
    
    // Verificar si necesitamos agregar un separador
    int need_separator = 0;
    if (dir_len > 0 && dir[dir_len - 1] != '/' && file_len > 0 && file[0] != '/')
        need_separator = 1;
    
    // Calcular tamaño total
    size_t total_len = dir_len + file_len + need_separator + 1;
    
    result = malloc(total_len);
    if (!result)
        return NULL;
    
    // Copiar el directorio
    strcpy(result, dir);
    
    // Agregar separador si es necesario
    if (need_separator)
        strcat(result, "/");
    
    // Agregar el nombre del archivo
    strcat(result, file);
    
    return result;
}

// Función generadora para archivos y directorios
char *file_generator(const char *text, int state)
{
    static DIR *dir;
    static char *directory;
    static char *filename;
    static int len;
    struct dirent *entry;
    char *full_path;
    char *result;
    
    // En el primer estado, inicializamos todo
    if (!state)
    {
        // Si hay un directorio abierto, cerrarlo
        if (dir)
        {
            closedir(dir);
            dir = NULL;
        }
        
        if (directory)
        {
            free(directory);
            directory = NULL;
        }
        
        // Obtener el directorio y el nombre base
        char *last_slash = strrchr(text, '/');
        
        if (last_slash)
        {
            // Hay un / en el path
            int dir_len = last_slash - text + 1;
            directory = malloc(dir_len + 1);
            if (!directory)
                return NULL;
            
            strncpy(directory, text, dir_len);
            directory[dir_len] = '\0';
            
            filename = last_slash + 1;
            
            // Si el directorio es vacío, usar "./"
            if (directory[0] == '\0')
            {
                free(directory);
                directory = ft_strdup("./");
            }
        }
        else
        {
            // No hay slash, buscamos en el directorio actual
            directory = ft_strdup("./");
            filename = (char *)text;
        }
        
        // Abrir el directorio
        dir = opendir(directory);
        if (!dir)
        {
            free(directory);
            directory = NULL;
            return NULL;
        }
        
        len = strlen(filename);
    }
    
    // Estado subsiguiente: continuar donde lo dejamos
    if (!dir)
        return NULL;
    
    while ((entry = readdir(dir)) != NULL)
    {
        // Ignorar "." y ".." si el usuario no los está buscando específicamente
        if ((strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) && 
            (len == 0 || (filename[0] != '.' && len == 1) || 
            (filename[0] == '.' && filename[1] != '.' && len == 1)))
            continue;
        
        // Comprobar si el nombre de archivo coincide con el prefijo
        if (strncmp(entry->d_name, filename, len) == 0)
        {
            // Construir el path completo
            if (strcmp(directory, "./") == 0 && strcmp(text, "./") != 0 && 
                strncmp(text, "./", 2) != 0)
            {
                // Si estamos en el directorio actual y el usuario no escribió ./, no lo incluimos
                full_path = ft_strdup(entry->d_name);
            }
            else
            {
                // Usar función segura para unir paths
                full_path = safe_path_join(directory, entry->d_name);
            }
            
            if (!full_path)
                continue;
            
            // Agregar un slash al final si es un directorio
            struct stat st;
            int is_dir = 0;
            
            // Comprobar si es un directorio
            if (full_path[0] == '/')
            {
                // Path absoluto
                if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
                    is_dir = 1;
            }
            else
            {
                // Path relativo
                char cwd[PATH_MAX];
                char *check_path;
                
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                {
                    free(full_path);
                    continue;
                }
                
                if (strncmp(full_path, "./", 2) == 0)
                    check_path = safe_path_join(cwd, full_path + 2);
                else
                    check_path = safe_path_join(cwd, full_path);
                
                if (check_path)
                {
                    if (stat(check_path, &st) == 0 && S_ISDIR(st.st_mode))
                        is_dir = 1;
                    free(check_path);
                }
            }
            
            if (is_dir)
            {
                // Es un directorio, agregar slash
                size_t path_len = strlen(full_path);
                result = malloc(path_len + 2);  // +2 para el slash y el null terminator
                if (!result)
                {
                    free(full_path);
                    continue;
                }
                strcpy(result, full_path);
                // Solo agregar slash si no termina ya en uno
                if (result[path_len - 1] != '/')
                    strcat(result, "/");
                free(full_path);
                return result;
            }
            
            return full_path;  // Archivo normal
        }
    }
    
    // No hay más coincidencias, limpieza
    closedir(dir);
    dir = NULL;
    free(directory);
    directory = NULL;
    
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
    else
        // En otro caso, completar archivos y directorios
        return rl_completion_matches(text, file_generator);
}

// Función para configurar el autocompletado
void setup_autocomplete(char **env)
{
    // Inicializar la lista de comandos
    initialize_command_names(env);
    
    // Configurar la función de autocompletado de readline
    rl_attempted_completion_function = command_completion;
}