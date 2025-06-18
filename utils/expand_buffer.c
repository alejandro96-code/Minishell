/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_buffer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 19:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/18 19:16:46 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función para redimensionar el buffer de resultado
int	resize_result_buffer(t_expand_state *state)
{
	char	*new_result;
	int		new_size;

	new_size = state->result_size * 2;
	new_result = malloc(new_size);
	if (!new_result)
		return (0);
	ft_strlcpy(new_result, state->result, state->result_pos + 1);
	free(state->result);
	state->result = new_result;
	state->result_size = new_size;
	return (1);
}

// Función para añadir un carácter al resultado
int	add_char_to_result(t_expand_state *state, char c)
{
	if (state->result_pos + 2 >= state->result_size)
	{
		if (!resize_result_buffer(state))
			return (0);
	}
	state->result[state->result_pos++] = c;
	return (1);
}

// Función para añadir una cadena al resultado
int	add_string_to_result(t_expand_state *state, const char *str)
{
	int	len;
	int	i;

	if (!str)
		return (1);
	len = ft_strlen(str);
	while (state->result_pos + len + 1 >= state->result_size)
	{
		if (!resize_result_buffer(state))
			return (0);
	}
	i = 0;
	while (str[i])
	{
		state->result[state->result_pos++] = str[i];
		i++;
	}
	return (1);
}
