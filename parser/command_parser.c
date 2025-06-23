/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_parser.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/23 14:50:01 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Verifica si la entrada está vacía o solo contiene espacios
static int	is_empty_or_whitespace(const char *str)
{
	while (*str)
	{
		if (*str != ' ' && *str != '\t' && *str != '\n')
			return (0);
		str++;
	}
	return (1);
}

// Verifica si las comillas están balanceadas
static int	validate_quotes(const char *input)
{
	int	i;
	int	in_single_quotes;
	int	in_double_quotes;

	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (input[i])
	{
		if (input[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		else if (input[i] == '"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		i++;
	}
	return (in_single_quotes || in_double_quotes);
}

// Inicializa la estructura t_command
static t_command	*init_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->argc = 0;
	cmd->is_builtin = 0;
	return (cmd);
}

// Configura los argumentos del comando
static int	setup_command_args(t_command *cmd, const char *input)
{
	cmd->argv = split_command_args((char *)input);
	if (!cmd->argv)
		return (0);
	while (cmd->argv[cmd->argc] != NULL)
		cmd->argc++;
	if (cmd->argc > 0 && cmd->argv[0])
		cmd->is_builtin = is_builtin_command(cmd->argv[0]);
	else
		cmd->is_builtin = 0;
	return (1);
}

// Procesa la entrada y genera la estructura t_command
t_command	*parse_command_input(const char *input, int *exit_status)
{
	t_command	*cmd;

	if (!input || is_empty_or_whitespace(input))
		return (NULL);
	if (validate_quotes(input))
	{
		ft_putstr_fd("minishell: unclosed quote\n", 2);
		*exit_status = 258;
		return (NULL);
	}
	if (!validate_redirections(input))
	{
		*exit_status = 258;
		ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
		return (NULL);
	}
	cmd = init_command();
	if (!cmd)
		return (NULL);
	if (!setup_command_args(cmd, input))
		return (free(cmd), NULL);
	if (cmd->argc == 0)
		return (free_command(cmd), NULL);
	return (cmd);
}
