/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 13:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/18 20:01:05 by alejandro        ###   ########.fr       */
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

char	*aux_split_redirection(char **str, char *start)
{
	char	redirect;

	redirect = **str;
	(*str)++;
	if (**str && is_redirection(**str) && **str == redirect)
	{
		(*str)++;
		return (ft_substr(start, 0, 2));
	}
	return (ft_substr(start, 0, 1));
}

// Actualiza el estado de las comillas durante el parsing
static void	update_quote_state(char c, int *in_single_quotes,
		int *in_double_quotes)
{
	if (c == '\'' && !*in_double_quotes)
		*in_single_quotes = !*in_single_quotes;
	else if (c == '"' && !*in_single_quotes)
		*in_double_quotes = !*in_double_quotes;
}

// Valida la sintaxis de las redirecciones
int	validate_redirections(const char *input)
{
	int	i;
	int	in_single_quotes;
	int	in_double_quotes;

	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (input[i])
	{
		update_quote_state(input[i], &in_single_quotes, &in_double_quotes);
		if (!in_single_quotes && !in_double_quotes && is_redirection(input[i]))
		{
			if (input[i + 1] && input[i] == input[i + 1])
				i++;
			i++;
			while (input[i] && (input[i] == ' ' || input[i] == '\t'))
				i++;
			if (input[i] && is_redirection(input[i--]))
				return (0);
		}
		i++;
	}
	return (1);
}
