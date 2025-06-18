/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument_extraction.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 20:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/18 19:50:58 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Maneja el estado de las comillas durante la extracción
static void	handle_quote_state(char **str, int *in_quotes, char *quote_char)
{
	if (!*in_quotes && (**str == '"' || **str == '\''))
	{
		*in_quotes = 1;
		*quote_char = **str;
	}
	else if (*in_quotes && **str == *quote_char)
		*in_quotes = 0;
}

// Calcula la longitud del argumento
static int	calculate_argument_length(char **str)
{
	char	*start;
	int		len;
	int		in_quotes;
	char	quote_char;

	start = *str;
	len = 0;
	in_quotes = 0;
	quote_char = 0;
	while (**str && (in_quotes || (**str != ' ' && **str != '\t')))
	{
		handle_quote_state(str, &in_quotes, &quote_char);
		if (!in_quotes && is_redirection(**str))
			break ;
		(*str)++;
		len++;
	}
	*str = start;
	return (len);
}

// Extrae el siguiente argumento de la cadena
char	*extract_next_argument(char **str)
{
	char	*start;
	char	*arg;
	int		len;

	skip_whitespace(str);
	start = *str;
	if (**str && is_redirection(**str))
		return (aux_split_redirection(str, start));
	len = calculate_argument_length(str);
	arg = ft_substr(start, 0, len);
	*str = start + len;
	return (arg);
}
