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

static void	fill_command(t_command *cmd, char *cleaned_input)
{
	int	i;

	i = 0;
	cmd->argv = ft_split(cleaned_input, ' ');
	if (!cmd->argv)
	{
		cmd->argc = 0;
		cmd->is_builtin = 0;
		return ;
	}
	while (cmd->argv[i] != NULL)
		i++;
	cmd->argc = i;
	cmd->is_builtin = (cmd->argc > 0 && cmd->argv[0]) ? is_builtin_command(cmd->argv[0]) : 0;
}

// Función que procesa la entrada y genera la estructura t_command
t_command	*parse_input(const char *input)
{
	t_command	*cmd;
	char		*cleaned_input;

	if (!input || is_empty_or_whitespace(input))
		return (NULL);
	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cleaned_input = clean_input((char *)input);
	if (!cleaned_input || is_empty_or_whitespace(cleaned_input))
	{
		free(cmd);
		free(cleaned_input);
		return (NULL);
	}
	fill_command(cmd, cleaned_input);
	free(cleaned_input);
	if (cmd->argc == 0)
	{
		free_command(cmd);
		return (NULL);
	}
	return (cmd);
}
