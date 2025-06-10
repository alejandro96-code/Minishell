/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:30:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*remove_quotes(char *str)
{
	size_t	len;
	char	*result;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"')
			|| (str[0] == '\'' && str[len - 1] == '\'')))
	{
		result = malloc(len - 1);
		if (!result)
			return (str);
		ft_strlcpy(result, str + 1, len - 2);
		result[len - 2] = '\0';
		free(str);
		return (result);
	}
	return (str);
}

static void	process_quote_char(char input_char, int *in_quotes, char *quote_char)
{
	if (!*in_quotes && (input_char == '"' || input_char == '\''))
	{
		*in_quotes = 1;
		*quote_char = input_char;
	}
	else if (*in_quotes && input_char == *quote_char)
	{
		*in_quotes = 0;
		*quote_char = 0;
	}
}

static void	init_clean_vars(size_t *i, size_t *j, int *in_quotes, char *quote_char)
{
	*i = 0;
	*j = 0;
	*in_quotes = 0;
	*quote_char = 0;
}

static void	process_char(char *input, char *cleaned_input, size_t *i, size_t *j)
{
	static int	in_quotes = 0;
	static char	quote_char = 0;

	if (!in_quotes && (input[*i] == '"' || input[*i] == '\''))
		process_quote_char(input[*i], &in_quotes, &quote_char);
	else if (in_quotes && input[*i] == quote_char)
		process_quote_char(input[*i], &in_quotes, &quote_char);
	else
		cleaned_input[(*j)++] = input[*i];
	(*i)++;
}

char	*clean_input(char *input)
{
	size_t	len;
	char	*cleaned_input;
	size_t	i;
	size_t	j;
	int		in_quotes;
	char	quote_char;

	if (!input)
		return (NULL);
	len = ft_strlen(input);
	cleaned_input = malloc(len + 1);
	if (!cleaned_input)
		return (NULL);
	init_clean_vars(&i, &j, &in_quotes, &quote_char);
	while (i < len)
		process_char(input, cleaned_input, &i, &j);
	cleaned_input[j] = '\0';
	return (cleaned_input);
}
