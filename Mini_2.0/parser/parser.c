/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 12:54:57 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función para verificar si el comando es un builtin (puedes agregar más)
int	is_builtin_command(const char *cmd)
{
	return (strcmp(cmd, "echo") == 0 || strcmp(cmd, "cd") == 0 || strcmp(cmd,
			"pwd") == 0);
}

// Función que procesa la entrada y genera la estructura t_command
t_command	*parse_input(const char *input)
{
	t_command	*cmd;
	char		*cleaned_input;
	int			i;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cleaned_input = clean_input(strdup(input));
	cmd->argv = ft_split(cleaned_input, ' ');
	free(cleaned_input);
	i = 0;
	while (cmd->argv && cmd->argv[i] != NULL)
	{
		i++;
	}
	cmd->argc = i;
	if (cmd->argv && cmd->argv[0])
		cmd->is_builtin = is_builtin(cmd->argv[0]);
	else
		cmd->is_builtin = 0;
	return (cmd);
}
