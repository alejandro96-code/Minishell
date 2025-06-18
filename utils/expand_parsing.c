/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_parsing.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 19:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/18 19:16:55 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Extrae el nombre de variable entre llaves {}
int	extract_braced_var_name(t_expand_state *state, char *var_name)
{
	int	i;

	i = 0;
	state->input_pos++;
	while (state->input[state->input_pos]
		&& state->input[state->input_pos] != '}' && i < 255)
	{
		if (!ft_isalnum(state->input[state->input_pos])
			&& state->input[state->input_pos] != '_')
			break ;
		var_name[i++] = state->input[state->input_pos++];
	}
	var_name[i] = '\0';
	if (state->input[state->input_pos] == '}')
		state->input_pos++;
	return (i > 0);
}

// Extrae el nombre de variable sin llaves
int	extract_var_name(t_expand_state *state, char *var_name)
{
	int	i;

	i = 0;
	while (state->input[state->input_pos]
		&& (ft_isalnum(state->input[state->input_pos])
			|| state->input[state->input_pos] == '_') && i < 255)
	{
		var_name[i++] = state->input[state->input_pos++];
	}
	var_name[i] = '\0';
	return (i > 0);
}
