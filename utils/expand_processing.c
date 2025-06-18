/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_processing.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 19:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/18 19:18:36 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Maneja la expansión de variables especiales ($ y ?)
static int	handle_special_variables(t_expand_state *state)
{
	char	*value;

	if (state->input[state->input_pos] == '?')
	{
		state->input_pos++;
		value = ft_itoa(state->exit_status);
		if (value)
		{
			add_string_to_result(state, value);
			free(value);
		}
		return (1);
	}
	if (state->input[state->input_pos] == '$')
	{
		state->input_pos++;
		value = ft_itoa(getpid());
		if (value)
		{
			add_string_to_result(state, value);
			free(value);
		}
		return (1);
	}
	return (0);
}

// Expande una variable de entorno
int	expand_variable_internal(t_expand_state *state)
{
	char	var_name[256];
	char	*value;
	int		name_len;

	state->input_pos++;
	if (handle_special_variables(state))
		return (1);
	if (state->input[state->input_pos] == '{')
		name_len = extract_braced_var_name(state, var_name);
	else
		name_len = extract_var_name(state, var_name);
	if (name_len == 0)
	{
		add_char_to_result(state, '$');
		return (1);
	}
	value = get_env_var(var_name, state->env);
	if (value)
		add_string_to_result(state, value);
	return (1);
}

// Procesa escape en comillas dobles
int	process_escape_in_double_quotes(t_expand_state *state)
{
	state->input_pos++;
	if (!state->input[state->input_pos])
	{
		add_char_to_result(state, '\\');
		return (1);
	}
	if (state->input[state->input_pos] == '"'
		|| state->input[state->input_pos] == '\\'
		|| state->input[state->input_pos] == '$'
		|| state->input[state->input_pos] == '`'
		|| state->input[state->input_pos] == '\n')
	{
		add_char_to_result(state, state->input[state->input_pos++]);
	}
	else
	{
		add_char_to_result(state, '\\');
		add_char_to_result(state, state->input[state->input_pos++]);
	}
	return (1);
}

// Función principal de procesamiento con comillas y variables
char	*process_quotes_and_vars(char *input, char **env, int exit_status)
{
	t_expand_state	state;

	if (!init_expand_state(&state, input, env, exit_status))
		return (NULL);
	while (state.input[state.input_pos])
	{
		process_character(&state, state.input[state.input_pos]);
	}
	state.result[state.result_pos] = '\0';
	return (state.result);
}
