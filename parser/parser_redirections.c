/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 22:43:18 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/27 17:25:30 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	skip_redirection_operator(const char *input, int i)
{
	int	j;

	j = i;
	if (input[j] && is_redirection(input[j]))
	{
		j++;
		if (input[j] && is_redirection(input[j]) && input[j] == input[j - 1])
			j++;
	}
	while (input[j] && ft_isspace(input[j]))
		j++;
	return (j);
}

int	skip_filename_with_quotes(const char *input, int i)
{
	int		j;
	char	quote;
	int		in_quotes;

	j = i;
	quote = '\0';
	in_quotes = 0;
	if (input[j] == '\"' || input[j] == '\'')
	{
		quote = input[j];
		in_quotes = 1;
		j++;
	}
	while (input[j])
	{
		if (in_quotes && input[j] == quote)
		{
			j++;
			break ;
		}
		else if (!in_quotes && (ft_isspace(input[j]) || input[j] == '|'))
			break ;
		j++;
	}
	return (j);
}

int	pipe_after_redirection_aux(const char *input, int i)
{
	int	j;

	j = skip_redirection_operator(input, i);
	if (input[j])
		j = skip_filename_with_quotes(input, j);
	while (input[j] && ft_isspace(input[j]))
		j++;
	if (input[j] == '|')
		return (1);
	return (1);
}

int	pipe_after_redirec(const char *input)
{
	int		i;
	char	quotes;
	int		in_quotes;

	i = 0;
	quotes = '\0';
	in_quotes = 0;
	while (input[i])
	{
		if (!in_quotes && (input[i] == '\"' || input[i] == '\''))
		{
			in_quotes = 1;
			quotes = input[i];
		}
		else if (in_quotes && (input[i] == quotes))
			in_quotes = 0;
		if (is_redirection(input[i]) && !in_quotes)
		{
			pipe_after_redirection_aux(input, i);
		}
		i++;
	}
	return (1);
}

void	print_error_in_pipe(void)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
}
