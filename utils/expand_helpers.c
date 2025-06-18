/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 19:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/18 19:16:50 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Inicializa el estado de expansión
int	init_expand_state(t_expand_state *state, char *input, char **env,
		int exit_status)
{
	if (!input)
		return (0);
	state->input = input;
	state->input_pos = 0;
	state->result_size = ft_strlen(input) * 2 + 256;
	state->result = malloc(state->result_size);
	if (!state->result)
		return (0);
	state->result_pos = 0;
	state->in_single_quotes = 0;
	state->in_double_quotes = 0;
	state->env = env;
	state->exit_status = exit_status;
	return (1);
}

// Procesa un carácter durante la expansión
void	process_character(t_expand_state *state, char current)
{
	if (current == '\'' && !state->in_double_quotes)
	{
		state->in_single_quotes = !state->in_single_quotes;
		state->input_pos++;
	}
	else if (current == '"' && !state->in_single_quotes)
	{
		state->in_double_quotes = !state->in_double_quotes;
		state->input_pos++;
	}
	else if (current == '$' && !state->in_single_quotes)
	{
		expand_variable_internal(state);
	}
	else if (current == '\\' && state->in_double_quotes)
	{
		process_escape_in_double_quotes(state);
	}
	else
	{
		add_char_to_result(state, current);
		state->input_pos++;
	}
}
