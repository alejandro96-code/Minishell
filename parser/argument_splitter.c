/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument_splitter.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/19 09:01:19 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Cuenta el número de argumentos en una cadena de comando
int	count_command_args(char *str)
{
	int	count;

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
