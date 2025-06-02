/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   comillas.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 09:50:58 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/02 19:16:21 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*quotes_clean(char *text, char quote)
{
	int		i;
	char	*var_env;
	char	*text_clean;

	var_env = malloc(1);
	var_env[0] = '\0';
	text_clean = malloc(1);
	text_clean[0] = '\0';
	i = 0;
	if (quote == '"')
	{
		while (text[i])
		{
			if (text[i] == '$')
			{
				while (text[i] != ' ')
					var_env = ft_strjoin_char(var_env, text[i++]);
				text_clean = ft_strjoin_s1_free(text_clean, var_env);
			}
			else
				text_clean = ft_strjoin_char(text_clean, text[i]);
			i++;
		}
	}
	else
	{
		while (text[i])
		{
			text_clean = ft_strjoin_char(text_clean, text[i]);
			i++;
		}
	}
	return (text_clean);
}

char	*input_quotes(char *input)
{
	int		i;
	char	*text_whit_quotes;
	char	quotes;
	char	*input_clean;

	quotes = NULL;
	i = 0;
	while (input[i])
	{
		if (input[i] == '"' || input[i] == '\'')
		{
			quotes = input[i];
			while (input[i + 1] == quotes)
			{
				text_whit_quotes = ft_strjoin_char(text_whit_quotes, input[i + 1]);
			}
			input_clean = quotes_clean(text_whit_quotes, quotes);
			free(text_whit_quotes);
		}
		else
			input_clean = ft_strjoin_char(input_clean, input[i]);
		i++;
	}
	return (input_clean);
}
