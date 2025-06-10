/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:35:15 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función para verificar si el comando es un builtin (puedes agregar más)
int	is_builtin_command(const char *cmd)
{
	if (!cmd)
		return (0);
	return ((ft_strncmp(cmd, "echo", 4) == 0 && ft_strlen(cmd) == 4) || 
			(ft_strncmp(cmd, "cd", 2) == 0 && ft_strlen(cmd) == 2) || 
			(ft_strncmp(cmd, "pwd", 3) == 0 && ft_strlen(cmd) == 3) || 
			(ft_strncmp(cmd, "export", 6) == 0 && ft_strlen(cmd) == 6) || 
			(ft_strncmp(cmd, "unset", 5) == 0 && ft_strlen(cmd) == 5) || 
			(ft_strncmp(cmd, "env", 3) == 0 && ft_strlen(cmd) == 3) || 
			(ft_strncmp(cmd, "exit", 4) == 0 && ft_strlen(cmd) == 4));
}

void	free_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
	}
	free(cmd);
}

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

// Función para verificar si las comillas están balanceadas
static int	quote_balance_check(const char *input)
{
	int		i;
	int		in_single_quotes;
	int		in_double_quotes;

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

// Función que procesa la entrada y genera la estructura t_command
t_command	*parse_input(const char *input)
{
	t_command	*cmd;

	if (!input || is_empty_or_whitespace(input))
		return (NULL);
	
	// Verificar comillas balanceadas
	if (quote_balance_check(input))
	{
		ft_putstr_fd("minishell: unclosed quote\n", 2);
		g_exit_status = 258;
		return (NULL);
	}
	
	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	
	// NO usar clean_input aquí - usar directamente ft_split_args que respeta comillas
	cmd->argv = ft_split_args((char *)input);
	if (!cmd->argv)
	{
		cmd->argc = 0;
		cmd->is_builtin = 0;
		free(cmd);
		return (NULL);
	}
	
	// Contar argumentos
	cmd->argc = 0;
	while (cmd->argv[cmd->argc] != NULL)
		cmd->argc++;
	
	cmd->is_builtin = (cmd->argc > 0 && cmd->argv[0]) ? is_builtin_command(cmd->argv[0]) : 0;
	
	if (cmd->argc == 0)
	{
		free_command(cmd);
		return (NULL);
	}
	return (cmd);
}
