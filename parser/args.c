/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 00:43:20 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 18:56:01 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_args(char *str)
{
	int		count;
	int		in_quotes;
	char	quote_type;

	count = 0;
	while (*str)
	{
		skip_spaces(&str);
		if (!*str)
			break ;
		count++;
		in_quotes = 0;
		while (*str && (in_quotes || !ft_isspace(*str)))
		{
			if (!in_quotes && (*str == '\'' || *str == '"'))
			{
				in_quotes = 1;
				quote_type = *str;
			}
			else if (in_quotes && *str == quote_type)
				in_quotes = 0;
			str++;
		}
	}
	return (count);
}

char	*extract_next_arg(char **str)
{
	char	*start;
	char	*result;
	int		len;
	int		in_quotes;

	skip_spaces(str);
	start = *str;
	len = 0;
	in_quotes = 0;
	while (**str && (in_quotes || !ft_isspace(**str)))
	{
		if (!in_quotes && (**str == '\'' || **str == '"'))
			in_quotes = **str;
		else if (in_quotes && **str == in_quotes)
			in_quotes = 0;
		(*str)++;
		len++;
	}
	result = ft_substr(start, 0, len);
	return (result);
}

void	skip_spaces(char **str)
{
	while (**str && ft_isspace(**str))
		(*str)++;
}

char	**free_args(char **args)
{
	int	i;

	if (!args)
		return (NULL);
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
	return (NULL);
}

char	**ft_split_args(char *str)
{
	char	**result;
	int		i;
	int		count_arg;

	if (!str)
		return (NULL);
	count_arg = count_args(str);
	result = malloc(sizeof(char *) * (count_arg + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < count_arg)
	{
		result[i] = extract_next_arg(&str);
		if (!result[i])
			return (free_args(result));
		i++;
	}
	result[i] = NULL;
	return (result);
}
