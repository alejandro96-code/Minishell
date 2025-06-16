/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 13:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/16 22:56:02 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Verifica si el comando es un builtin
int	is_builtin_command(const char *cmd)
{
	if (!cmd)
		return (0);
	return ((ft_strncmp(cmd, "echo", 4) == 0 && ft_strlen(cmd) == 4)
		|| (ft_strncmp(cmd, "cd", 2) == 0 && ft_strlen(cmd) == 2)
		|| (ft_strncmp(cmd, "pwd", 3) == 0 && ft_strlen(cmd) == 3)
		|| (ft_strncmp(cmd, "export", 6) == 0 && ft_strlen(cmd) == 6)
		|| (ft_strncmp(cmd, "unset", 5) == 0 && ft_strlen(cmd) == 5)
		|| (ft_strncmp(cmd, "env", 3) == 0 && ft_strlen(cmd) == 3)
		|| (ft_strncmp(cmd, "exit", 4) == 0 && ft_strlen(cmd) == 4));
}
int	is_redirection(char c)
{
	if (c == '<' || c == '>')
		return (1);
	return (0);
}

char *aux_split_redirection(char **str, char *start)
{
	char	*arg;
	char	redirect;
	
	redirect = **str;
	(*str)++;
	if (**str && is_redirection(**str) && **str == redirect)
	{
		(*str)++;
		return (arg = ft_substr(start, 0, 2));
	}
	return (arg = ft_substr(start, 0, 1));
}