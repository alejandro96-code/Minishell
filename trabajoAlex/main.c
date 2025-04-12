/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 16:53:35 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/12 17:13:22 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
char	*get_prompt(char ** env)
{
	char *text;
	char cwd[1024];

	text = find_user(env);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return (free (text), NULL);
	}
    text = ft_strjoin(YELLOW, find_user(env));
    text = ft_strjoin_s1_free(text, RED);
    text = ft_strjoin_s1_free(text, cwd);
    text = ft_strjoin_s1_free(text, RESET);
    text = ft_strjoin_s1_free(text, ": ");
    return (text);
}

//limpia, tokeniza, ejecuta y libera memoria
void process_input(char *input, char ***env)
{
    static int last_exit_status = 0;
    
    // Verificar si la entrada contiene pipes
    if (strchr(input, '|') != NULL)
    {
        last_exit_status = execute_pipeline(input, *env);
        free(input);
        return;
    }
    
    char *cleaned_input = clean_input(input);
    free(input);
    
    char **args = ft_split(cleaned_input, ' ');
    int cont = 0;
    while (args && args[cont])
    {
        // Expandir variables, incluyendo $?
        char *expanded = expand_variable(args[cont], *env, last_exit_status);
        free(args[cont]);
        args[cont] = expanded;
        
        // Quitar comillas
        args[cont] = remove_quotes(args[cont]);
        cont++;
    }
    
    // Expandir wildcards (*)
    int num_args = cont;
    args = expand_wildcards_in_args(args, &num_args);
    
    // Manejar redirecciones
    handle_redirections(&args, *env);
    
    if (args && args[0])
    {
        if (is_builtin(args[0]))
            last_exit_status = execute_builtin(args, env);
        else
        {
            execute_external(args, *env);
            last_exit_status = 0; // Simplificado, deberías capturar el estado real
        }
    }
    
    cont = 0;
    while (args && args[cont])
        free(args[cont++]);
    free(args);
    free(cleaned_input);
}

// Función principal (Inicia, muestra el mensaje y entra al bucle)
int main(int argc, char **argv, char **envp)
{
    char *input = NULL;
    char **env = copy_env(envp);

    (void)argc;
    (void)argv;

    printf("Minishell builtins test mode. Ctrl+C to get new prompt, Ctrl+D to exit.\n");
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

    int cont = 0;
    while (env[cont])
        free(env[cont++]);
    free(env);
    return 0;
}
