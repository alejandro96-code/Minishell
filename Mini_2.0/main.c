/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/11 13:51:35 by alejandro        ###   ########.fr       */
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

// Función para limpiar las comillas del input completo
char	*clean_input(char *input)
{
	size_t	len;
	char	*cleaned_input;
	size_t	cont_input_1;
	size_t	cont_input_2;

	if (!input)
		return (NULL);
	len = strlen(input);
	cleaned_input = malloc(len + 1);
	cont_input_1 = 0;
	cont_input_2 = 0;
	while (cont_input_1 < len)
	{
		if (input[cont_input_1] != '"' && input[cont_input_1] != '\'')
			cleaned_input[cont_input_2++] = input[cont_input_1];
		cont_input_1++;
	}
	cleaned_input[cont_input_2] = '\0';
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

	if (strchr(input, '|') != NULL)
	{
		execute_pipeline(input, *env);
		free(input);
		return ;
	}
	cmd = parse_input(input);
	if (!cmd)
	{
		free(input);
		return ;
	}
	process_single_command(cmd, env);
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
