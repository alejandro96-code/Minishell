#include "minishell.h"

// Copia el envp al entorno local
char **copy_env(char **envp)
{
    int cont = 0;
    while (envp[cont])
        cont++;
    
    char **copy = malloc((cont + 1) * sizeof(char *));
    cont = 0;
    while (envp[cont])
    {
        copy[cont] = ft_strdup(envp[cont]);
        cont++;
    }
    copy[cont] = NULL;
    return copy;
}

// Función para eliminar comillas del inicio y final del string si existen
char *remove_quotes(char *str)
{
    if (!str)
        return NULL;

    size_t len = ft_strlen(str);

    // Si el string tiene comillas al inicio y al final, las eliminamos
    if (len >= 2 && ((str[0] == '"' && str[len-1] == '"') || 
                     (str[0] == '\'' && str[len-1] == '\'')))
    {
        // Crear una copia sin las comillas
        char *result = malloc(len - 1); // -2 por las comillas, +1 por el null terminator
        if (!result)
            return str; // Si falla, devolver el original
        
        ft_strlcpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
        
        free(str); // Liberar el string original
        return result;
    }
    
    // Si las comillas están desbalanceadas, no hacemos nada
    return str;
}

// Función para limpiar las comillas del input completo
char *clean_input(char *input)
{
    if (!input)
        return NULL;

    size_t len = strlen(input);
    char *cleaned_input = malloc(len + 1);
    size_t cont_input_1 = 0;
    size_t cont_input_2 = 0;

    while (cont_input_1 < len)
    {
        if (input[cont_input_1] != '"' && input[cont_input_1] != '\'')
            cleaned_input[cont_input_2++] = input[cont_input_1];
        cont_input_1++;
    }

    cleaned_input[cont_input_2] = '\0';
    return cleaned_input;
}

// Función para imprimir el prompt
char *get_prompt(char **env)
{
    char *username = find_user(env);
    char cwd[1024];
    char *prompt = NULL;
    
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        free(username);
        return NULL;
    }
    
    prompt = ft_strjoin(YELLOW, username);
    free(username); // Liberar memoria después de usar username
    
    prompt = ft_strjoin_s1_free(prompt, RED);
    prompt = ft_strjoin_s1_free(prompt, cwd);
    prompt = ft_strjoin_s1_free(prompt, RESET);
    prompt = ft_strjoin_s1_free(prompt, ": ");
    
    return prompt;
}

//limpia, tokeniza, ejecuta y libera memoria
void process_input(char *input, char ***env)
{ 
    // Verificar si la entrada contiene pipes
    if (strchr(input, '|') != NULL)
    {
        execute_pipeline(input, *env);
        free(input);
        return;
    }
    
    // Usar la función parse_input para obtener la estructura t_command
    t_command *cmd = parse_input(input);
    if (!cmd)
    {
        free(input);
        return;
    }
    
    // Expandir variables y wildcards en los argumentos
    int i = 0;
    while (i < cmd->argc)
    {
        // Expandir variables
        char *expanded = expand_variable(cmd->argv[i], *env);
        free(cmd->argv[i]);
        cmd->argv[i] = expanded;
        
        // Quitar comillas
        cmd->argv[i] = remove_quotes(cmd->argv[i]);
        i++;
    }
    
    // Expandir wildcards (*)
    cmd->argv = expand_wildcards_in_args(cmd->argv, &cmd->argc);
    
    // Manejar redirecciones
    handle_redirections(&cmd->argv, *env);
    
    if (cmd->argv && cmd->argv[0])
    {
        if (cmd->is_builtin)
            execute_builtin(cmd->argv, env);
        else
            execute_external(cmd->argv, *env);
    }
    
    // Liberar memoria
    free(input);
    free(cmd);
}

// Función principal (Inicia, muestra el mensaje y entra al bucle)
int main(int argc, char **argv, char **envp)
{
    char *input = NULL;
    char **env = copy_env(envp);

    (void)argc;
    (void)argv;

    setup_autocomplete(env);
    setup_signal_handlers(); // Configurar los manejadores de señales

    
    while (1)
    {
        input = readline(get_prompt(env));
        
        // Manejar Ctrl+D (EOF)
        if (!input)
        {
            printf("exit\n");
            break;
        }
        
        if (input && *input)
        {
            add_history(input);
            process_input(input, &env);
        }
        else
        {
            free(input);
        }
    }
    
    reset_signal_handlers(); // Restaurar el comportamiento por defecto
    free_command_names();
    int cont = 0;
    while (env[cont])
        free(env[cont++]);
    free(env);
    return 0;
}