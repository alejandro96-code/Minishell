/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:00:00 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/19 09:04:28 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Salta espacios en blanco y tabulaciones
void	skip_whitespace(char **str)
{
	while (**str && (**str == ' ' || **str == '\t'))
		(*str)++;
}

// Procesa redirecciones y avanza el puntero
void	skip_redirection(char **str)
{
	char	redir_type;

	redir_type = **str;
	(*str)++;
	if (**str && is_redirection(**str) && redir_type == **str)
		(*str)++;
}

// Procesa un argumento regular (con comillas) y avanza el puntero
void	skip_regular_arg(char **str)
{
	int		in_quotes;
	char	quote_char;

	in_quotes = 0;
	quote_char = 0;
	while (**str && (in_quotes || (**str != ' ' && **str != '\t'
				&& !is_redirection(**str))))
	{
		if (!in_quotes && (**str == '"' || **str == '\''))
		{
			in_quotes = 1;
			quote_char = **str;
		}
		else if (in_quotes && **str == quote_char)
			in_quotes = 0;
		(*str)++;
	}
}

// Procesa un argumento y avanza el puntero
void	skip_current_arg(char **str)
{
	if (is_redirection(**str))
		skip_redirection(str);
	else
		skip_regular_arg(str);
}
