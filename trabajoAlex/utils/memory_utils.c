#include "../minishell.h"

/**
 * Libera un puntero y lo establece a NULL para evitar uso después de liberar
 * @param ptr Puntero a un puntero que debe ser liberado
 */
void safe_free(void **ptr)
{
    if (ptr && *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
}

/**
 * Libera una matriz de strings (como args o env)
 * @param array Matriz de strings a liberar
 */
void free_string_array(char ***array)
{
    int i;

    if (!array || !*array)
        return;

    i = 0;
    while ((*array)[i])
    {
        safe_free((void **)&((*array)[i]));
        i++;
    }
    safe_free((void **)array);
}

/**
 * Libera de manera segura una estructura t_command
 * @param cmd Puntero a la estructura t_command
 */
void free_command(t_command **cmd)
{
    if (!cmd || !*cmd)
        return;

    free_string_array(&((*cmd)->argv));
    safe_free((void **)cmd);
}

/**
 * Asigna memoria y verifica si fue exitosa. Si falla, muestra un error
 * @param size Tamaño en bytes a asignar
 * @return Puntero a la memoria asignada o NULL si falla
 */
void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        perror("malloc error");
        return NULL;
    }
    return ptr;
}

/**
 * Duplica una cadena de manera segura (verificando errores)
 * @param str Cadena a duplicar
 * @return Puntero a la nueva cadena o NULL si falla
 */
char *safe_strdup(const char *str)
{
    if (!str)
        return NULL;
    
    char *dup = strdup(str);
    if (!dup)
        perror("strdup error");
    
    return dup;
}

/**
 * Crea una copia del entorno de manera segura
 * @param envp Entorno original
 * @return Copia del entorno o NULL si falla
 */
char **safe_copy_env(char **envp)
{
    int cont = 0;
    char **copy;

    if (!envp)
        return NULL;

    // Contar elementos
    while (envp[cont])
        cont++;
    
    // Asignar memoria
    copy = safe_malloc((cont + 1) * sizeof(char *));
    if (!copy)
        return NULL;
    
    // Copiar cada elemento
    cont = 0;
    while (envp[cont])
    {
        copy[cont] = safe_strdup(envp[cont]);
        if (!copy[cont])
        {
            // Liberar lo que ya se asignó si hay un error
            free_string_array(&copy);
            return NULL;
        }
        cont++;
    }
    copy[cont] = NULL;
    
    return copy;
}