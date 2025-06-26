/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 22:43:18 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/26 23:51:57 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	pipe_after_redirection_aux(const char *input, int i)
{
	int j;

	j = i;
	printf("%c\n",input[i]);
	while (input[j] && ft_isspace(input[j]))
		j++;
	if (input[j] == '|')
		return (1);
	return (0);
}

int	pipe_after_redirec(const char *input)
{
	int	i;
	char quotes;
	int in_quotes;

	i = 0;
	quotes = '\0';
	in_quotes = 0;
	while (input[i])
	{
		if (!in_quotes &&(input[i] == '\"' || input[i] == '\''))
		{
			in_quotes = 1;
			quotes = input[i];
		}
		else if (in_quotes && (input[i] == quotes))
			in_quotes = 0;
		if (is_redirection(input[i]) && !in_quotes)
		{
			if (!pipe_after_redirection_aux(input, i))
			return (0);
		}
		
		i++;
	}
	return (1);
}

void print_error_in_pipe(void)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
}
