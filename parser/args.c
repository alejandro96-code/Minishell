/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/13 19:21:57 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	skip_spaces(char **str)
{
	while (**str && (**str == ' ' || **str == '\t'))
		(*str)++;
}

int	count_args(char *str)
{
	int	count;
	int	in_quotes;
	char	quote_char;

	count = 0;
	while (*str)
	{
		skip_spaces(&str);
		if (!*str)
			break ;
		count++;
		in_quotes = 0;
		quote_char = 0;
		while (*str && (in_quotes || (*str != ' ' && *str != '\t')))
		{
			if (!in_quotes && (*str == '"' || *str == '\''))
			{
				in_quotes = 1;
				quote_char = *str;
			}
			else if (in_quotes && *str == quote_char)
				in_quotes = 0;
			str++;
		}
	}
	return (count);
}

char	*extract_next_arg(char **str)
{
	char	*start;
	char	*arg;
	int		len;
	int		in_quotes;
	char	quote_char;

	skip_spaces(str);
	start = *str;
	len = 0;
	in_quotes = 0;
	quote_char = 0;
	while (**str && (in_quotes || (**str != ' ' && **str != '\t')))
	{
		if (!in_quotes && (**str == '"' || **str == '\''))
		{
			in_quotes = 1;
			quote_char = **str;
		}
		else if (in_quotes && **str == quote_char)
			in_quotes = 0;
		(*str)++;
		len++;
	}
	arg = ft_substr(start, 0, len);
	return (arg);
}

char	**ft_split_args(char *str)
{
	char	**result;
	int		arg_count;
	int		i;

	if (!str)
		return (NULL);
	arg_count = count_args(str);
	result = malloc(sizeof(char *) * (arg_count + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < arg_count)
	{
		result[i] = extract_next_arg(&str);
		if (!result[i])
			return (free_args(result));
		i++;
	}
	result[i] = NULL;
	return (result);
}
