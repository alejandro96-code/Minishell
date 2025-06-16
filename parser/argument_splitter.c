/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument_splitter.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/16 22:38:57 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Salta espacios en blanco y tabulaciones
void	skip_whitespace(char **str)
{
	while (**str && (**str == ' ' || **str == '\t'))
		(*str)++;
}

// Procesa un argumento y avanza el puntero
static void	skip_current_arg(char **str)
{
	int		in_quotes;
	char	quote_char;
	char	redir_tipe;

	in_quotes = 0;
	quote_char = 0;
	if (is_redirection(**str))
	{
		redir_tipe = **str;
		(*str)++;
		if (**str && is_redirection(**str) && redir_tipe == **str)
			(*str)++;
		return ;
	}
	while (**str && (in_quotes || (**str != ' ' && **str != '\t' && !is_redirection(**str))))
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

// Cuenta el número de argumentos en una cadena de comando
int	count_command_args(char *str)
{
	int		count;

	count = 0;
	while (*str)
	{
		skip_whitespace(&str);
		if (!*str)
			break ;
		count++;
		skip_current_arg(&str);
	}
	return (count);
}

// Extrae el siguiente argumento de la cadena
char	*extract_next_argument(char **str)
{
	char	*start;
	char	*arg;
	int		len;
	int		in_quotes;
	char	quote_char;

	skip_whitespace(str);
	start = *str;
	len = 0;
	in_quotes = 0;
	quote_char = 0;
	if (**str && is_redirection(**str))
		return (aux_split_redirection(str, start));	
	while (**str && (in_quotes || (**str != ' ' && **str != '\t')))
	{
		if (!in_quotes && (**str == '"' || **str == '\''))
		{
			in_quotes = 1;
			quote_char = **str;
		}
		else if (in_quotes && **str == quote_char)
			in_quotes = 0;
		else if (!in_quotes && is_redirection(**str)) // esto esta en caso de encontrar redireccion
            break;
		(*str)++;
		len++;
	}
	arg = ft_substr(start, 0, len);
	return (arg);
}

// Divide una cadena de comando en argumentos individuales
char	**split_command_args(char *str)
{
	char	**result;
	int		arg_count;
	int		i;

	if (!str)
		return (NULL);
	arg_count = count_command_args(str);
	result = malloc(sizeof(char *) * (arg_count + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < arg_count)
	{
		result[i] = extract_next_argument(&str);
		if (!result[i])
			return (free_args(result));
		i++;
	}
	result[i] = NULL;
	return (result);
}
