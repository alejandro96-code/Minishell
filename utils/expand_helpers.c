/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 19:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/29 03:37:27 by dgasco-g         ###   ########.fr       */
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
	state->result = ft_calloc(1, state->result_size);
	if (!state->result)
		return (0);
	state->result_pos = 0;
	state->in_single_quotes = 0;
	state->in_double_quotes = 0;
	state->env = env;
	state->exit_status = exit_status;
	return (1);
}

int	handle_quotes(t_expand_state *state, char current)
{
	if (current == '\'' && !state->in_double_quotes)
	{
		state->in_single_quotes = !state->in_single_quotes;
		state->input_pos++;
		return (1);
	}
	else if (current == '"' && !state->in_single_quotes)
	{
		state->in_double_quotes = !state->in_double_quotes;
		state->input_pos++;
		return (1);
	}
	return (0);
}

int	handle_dollar_and_escape(t_expand_state *state, char current)
{
	if (current == '$' && !state->in_single_quotes)
	{
		if (state->input[state->input_pos + 1] == '"')
		{
			state->input_pos++;
			process_character(state, state->input[state->input_pos]);
		}
		else
		{
			expand_variable_internal(state);
		}
		return (1);
	}
	else if (current == '\\' && state->in_double_quotes)
	{
		process_escape_in_double_quotes(state);
		return (1);
	}
	return (0);
}

int	handle_special_chars(t_expand_state *state, char current)
{
	if (handle_quotes(state, current))
		return (1);
	if (handle_dollar_and_escape(state, current))
		return (1);
	return (0);
}

// Procesa un carácter durante la expansión
void	process_character(t_expand_state *state, char current)
{
	if (handle_special_chars(state, current))
		return ;
	add_char_to_result(state, current);
	state->input_pos++;
}
