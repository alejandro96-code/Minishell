/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 16:53:35 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/13 12:39:45 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Copia el envp al entorno local
char **copy_env(char **envp)
{
    return safe_copy_env(envp);
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
    char *text = NULL;
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        free(username);
        return NULL;
    }
    
    text = ft_strjoin(YELLOW, username);
    free(username);  // Liberar la memoria del username después de usarlo
    
    text = ft_strjoin_s1_free(text, RED);
    text = ft_strjoin_s1_free(text, cwd);
    text = ft_strjoin_s1_free(text, RESET);
    text = ft_strjoin_s1_free(text, ": ");
    
    return text;
}

//limpia, tokeniza, ejecuta y libera memoria
// Limpia, tokeniza, ejecuta y libera memoria de forma segura
void process_input(char *input, char ***env)
{
    static int last_exit_status = 0;
    char *cleaned_input = NULL;
    char **args = NULL;
    char **expanded_args = NULL;
    int cont = 0;
    int num_args = 0;
    
    if (!input || !env || !*env)
        return;
    
    // Verificar si la entrada contiene pipes
    if (strchr(input, '|') != NULL)
    {
        last_exit_status = execute_pipeline(input, *env);
        safe_free((void **)&input);
        return;
    }
    
    cleaned_input = clean_input(input);
    safe_free((void **)&input);
    
    if (!cleaned_input)
        return;
    
    args = ft_split(cleaned_input, ' ');
    safe_free((void **)&cleaned_input);
    
    if (!args)
        return;
    
    // Contar argumentos y expandir variables
    while (args[cont])
    {
        // Expandir variables, incluyendo $?
        char *expanded = expand_variable(args[cont], *env, last_exit_status);
        safe_free((void **)&args[cont]);
        args[cont] = expanded;
        
        if (args[cont])
        {
            // Quitar comillas
            args[cont] = remove_quotes(args[cont]);
        }
        cont++;
    }
    
    // Expandir wildcards (*)
    num_args = cont;
    expanded_args = expand_wildcards_in_args(args, &num_args);
    
    // Si expand_wildcards_in_args devuelve un nuevo array, liberar el original
    if (expanded_args != args)
    {
        free_string_array(&args);
        args = expanded_args;
    }
    
    // Manejar redirecciones
    handle_redirections(&args, *env);
    
    if (args && args[0])
    {
        if (is_builtin(args[0]))
            last_exit_status = execute_builtin(args, env);
        else
        {
            execute_external(args, *env);
            last_exit_status = 0;
        }
    }
    
    // Liberar todos los args
    free_string_array(&args);
}

// Función principal (Inicia, muestra el mensaje y entra al bucle)
int main(int argc, char **argv, char **envp)
{
    char *input = NULL;
    char *prompt = NULL;
    char **env = NULL;

    (void)argc;
    (void)argv;

    env = safe_copy_env(envp);
    if (!env)
    {
        fprintf(stderr, "Error: No se pudo copiar el entorno.\n");
        return 1;
    }

    printf("Minishell builtins test mode. Ctrl+C to get new prompt, Ctrl+D to exit.\n");
    while (1)
    {
        prompt = get_prompt(env);
        input = readline(prompt);
        safe_free((void **)&prompt);
        
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
            // No hacer free de input aquí, ya lo hace process_input
        }
        else
        {
            safe_free((void **)&input);
        }
    }

    // Liberar el entorno
    free_string_array(&env);
    clear_history();
    
    return 0;
}
