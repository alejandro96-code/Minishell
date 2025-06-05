/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:35:17 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Copia el envp al entorno local
char	**copy_env(char **envp)
{
	int		cont;
	char	**copy;

	cont = 0;
	while (envp[cont])
		cont++;
	copy = malloc((cont + 1) * sizeof(char *));
	cont = 0;
	while (envp[cont])
	{
		copy[cont] = ft_strdup(envp[cont]);
		cont++;
	}
	copy[cont] = NULL;
	return (copy);
}

// Función para eliminar comillas del inicio y final del string si existen
char	*remove_quotes(char *str)
{
	size_t	len;
	char	*result;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') || (str[0] == '\''
				&& str[len - 1] == '\'')))
	{
		result = malloc(len - 1);
		if (!result)
			return (str);
		ft_strlcpy(result, str + 1, len - 2);
		result[len - 2] = '\0';
		free(str);
		return (result);
	}
	return (str);
}

static void	process_quote_char(char input_char, int *in_quotes, char *quote_char)
{
	if (!*in_quotes && (input_char == '"' || input_char == '\''))
	{
		*in_quotes = 1;
		*quote_char = input_char;
	}
	else if (*in_quotes && input_char == *quote_char)
	{
		*in_quotes = 0;
		*quote_char = 0;
	}
}

// Función para limpiar las comillas del input completo preservando espacios
char	*clean_input(char *input)
{
	size_t	len;
	char	*cleaned_input;
	size_t	i;
	size_t	j;
	int		in_quotes;
	char	quote_char;

	if (!input)
		return (NULL);
	len = ft_strlen(input);
	cleaned_input = malloc(len + 1);
	if (!cleaned_input)
		return (NULL);
	i = 0;
	j = 0;
	in_quotes = 0;
	quote_char = 0;
	while (i < len)
	{
		if (!in_quotes && (input[i] == '"' || input[i] == '\''))
			process_quote_char(input[i], &in_quotes, &quote_char);
		else if (in_quotes && input[i] == quote_char)
			process_quote_char(input[i], &in_quotes, &quote_char);
		else
			cleaned_input[j++] = input[i];
		i++;
	}
	cleaned_input[j] = '\0';
	return (cleaned_input);
}

// Función para imprimir el prompt
char	*get_prompt(char **env)
{
	char	*username;
	char	cwd[1024];
	char	*prompt;

	username = find_user(env);
	prompt = NULL;
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		free(username);
		return (NULL);
	}
	prompt = ft_strjoin(YELLOW, username);
	free(username);
	prompt = ft_strjoin_s1_free(prompt, RED);
	prompt = ft_strjoin_s1_free(prompt, cwd);
	prompt = ft_strjoin_s1_free(prompt, RESET);
	prompt = ft_strjoin_s1_free(prompt, ": ");
	return (prompt);
}

static void	process_single_command(t_command *cmd, char ***env)
{
	int		i;
	char	*expanded;

	i = 0;
	while (i < cmd->argc)
	{
		expanded = expand_variable(cmd->argv[i], *env);
		free(cmd->argv[i]);
		cmd->argv[i] = remove_quotes(expanded);
		i++;
	}
	cmd->argv = expand_wildcards_in_args(cmd->argv, &cmd->argc);
	handle_redirections(&cmd->argv, *env);
	if (cmd->argv && cmd->argv[0])
	{
		if (cmd->is_builtin)
			execute_builtin(cmd->argv, env);
		else
			execute_external(cmd->argv, *env);
	}
}

// limpia, tokeniza, ejecuta y libera memoria
void	process_input(char *input, char ***env)
{
	t_command	*cmd;
	
	// Reset signal flag before processing command
	g_signal_received = 0;
	
	cmd = parse_input(input);
	if (ft_strchr(input, '|') != NULL)
	{
		execute_pipeline(input, *env);
		free(input);
		return ;
	}
	if (!cmd)
	{
		free(input);
		return ;
	}
	process_single_command(cmd, env);
	
	// Check if signal was received during command execution
	if (g_signal_received == SIGINT)
	{
		// Set exit status to 130 like bash does for SIGINT
		// You might want to store this in a global variable
		g_signal_received = 0;
	}
	else if (g_signal_received == SIGQUIT)
	{
		// Set exit status to 131 like bash does for SIGQUIT
		g_signal_received = 0;
	}
	
	free(input);
	free(cmd);
}

static void	handle_input(char *input, char ***env)
{
	if (*input)
	{
		add_history(input);
		process_input(input, env);
	}
	else
		free(input);
}

static void	cleanup_and_exit(char **env)
{
	int	i;

	i = 0;
	reset_signal_handlers();
	free_command_names();
	while (env[i])
		free(env[i++]);
	free(env);
}

// Función principal (Inicia, muestra el mensaje y entra al bucle)
int	main(int argc, char **argv, char **envp)
{
	char	*input;
	char	**env;

	(void)argc;
	(void)argv;
	env = copy_env(envp);
	setup_autocomplete(env);
	setup_signal_handlers();
	while (1)
	{
		g_signal_received = 0;
		input = readline(get_prompt(env));
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		handle_input(input, &env);
	}
	cleanup_and_exit(env);
	return (0);
}
