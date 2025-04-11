/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 16:53:35 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/07 20:38:17 by dgasco-g         ###   ########.fr       */
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
	text = ft_strjoin_s1_free(text, cwd);
    text = ft_strjoin_s1_free(text, ": ");
    return (text);
}

//limpia, tokeniza, ejecuta y libera memoria
void process_input(char *input, char ***env)
{
    char *cleaned_input = clean_input(input);
    free(input);
    char **args = ft_split(cleaned_input, ' ');
    int cont = 0;
    while (args && args[cont])
    {
        args[cont] = remove_quotes(args[cont]);
        cont++;
    }
    if (args && args[0])
    {
        if (is_builtin(args[0]))
            execute_builtin(args, env);
        else
            execute_external(args, *env);
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

    printf("Minishell builtins test mode. Ctrl+C to exit.\n");
    while (1)
    {
        input = readline(get_prompt(env));
        if (input && *input)
            add_history(input);
        process_input(input, &env);
    }

    free(input);
    int cont = 0;
    while (env[cont])
        free(env[cont++]);
    free(env);
    return 0;
}
