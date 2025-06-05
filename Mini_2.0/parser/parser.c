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
	return (ft_strncmp(cmd, "echo", 4) == 0 || ft_strncmp(cmd, "cd", 2) == 0 || ft_strncmp(cmd,
			"pwd", 3) == 0 || ft_strncmp(cmd, "export", 6) == 0 || ft_strncmp(cmd,
			"unset", 5) == 0 || ft_strncmp(cmd, "env", 3) == 0 || ft_strncmp(cmd,
			"exit", 4) == 0);
}

static void	fill_command(t_command *cmd, char *cleaned_input)
{
	int	i;

	i = 0;
	cmd->argv = ft_split(cleaned_input, ' ');
	while (cmd->argv && cmd->argv[i] != NULL)
		i++;
	cmd->argc = i;
	cmd->is_builtin = (cmd->argv && cmd->argv[0]) ? is_builtin_command(cmd->argv[0]) : 0;
}

// Función que procesa la entrada y genera la estructura t_command
t_command	*parse_input(const char *input)
{
	t_command	*cmd;
	char		*cleaned_input;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cleaned_input = clean_input((char *)input);
	if (!cleaned_input)
	{
		free(cmd);
		return (NULL);
	}
	fill_command(cmd, cleaned_input);
	free(cleaned_input);
	return (cmd);
}
