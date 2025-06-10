/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 19:27:09 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Estructura para el estado del parser de expansión
typedef struct s_expand_state
{
	char	*input;
	char	*result;
	int		input_pos;
	int		result_pos;
	int		result_size;
	int		in_single_quotes;
	int		in_double_quotes;
	char	**env;
	int		exit_status;
}	t_expand_state;

// Función para redimensionar el buffer de resultado
static int	resize_result_buffer(t_expand_state *state)
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
static int	add_char_to_result(t_expand_state *state, char c)
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
static int	add_string_to_result(t_expand_state *state, const char *str)
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

// Función para encontrar el valor de una variable de entorno
char	*get_env_value(char *var_name, char **env)
{
	int		i;
	size_t	len;

	if (!var_name || !env)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], var_name, len) == 0 && env[i][len] == '=')
			return (env[i] + len + 1);
		i++;
	}
	return (NULL);
}

// Función para extraer el nombre de variable con llaves ${VAR}
static int	extract_braced_var_name(t_expand_state *state, char *var_name)
{
	int	i;

	i = 0;
	state->input_pos++; // Saltar '{'
	while (state->input[state->input_pos] && state->input[state->input_pos] != '}' && i < 255)
	{
		if (!ft_isalnum(state->input[state->input_pos]) && state->input[state->input_pos] != '_')
			break;
		var_name[i++] = state->input[state->input_pos++];
	}
	var_name[i] = '\0';
	if (state->input[state->input_pos] == '}')
		state->input_pos++; // Saltar '}'
	return (i);
}

// Función para extraer el nombre de variable normal $VAR
static int	extract_var_name(t_expand_state *state, char *var_name)
{
	int	i;

	i = 0;
	while (state->input[state->input_pos] && 
		   (ft_isalnum(state->input[state->input_pos]) || state->input[state->input_pos] == '_') && 
		   i < 255)
	{
		var_name[i++] = state->input[state->input_pos++];
	}
	var_name[i] = '\0';
	return (i);
}

// Función para expandir una variable
static int	expand_variable_internal(t_expand_state *state)
{
	char	var_name[256];
	char	*value;
	int		name_len;

	state->input_pos++; // Saltar '$'
	
	// Casos especiales
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
	
	// Variable con llaves ${VAR}
	if (state->input[state->input_pos] == '{')
	{
		name_len = extract_braced_var_name(state, var_name);
	}
	else
	{
		// Variable normal $VAR
		name_len = extract_var_name(state, var_name);
	}
	
	if (name_len == 0)
	{
		add_char_to_result(state, '$');
		return (1);
	}
	
	value = get_env_value(var_name, state->env);
	if (value)
		add_string_to_result(state, value);
	
	return (1);
}

// Función para procesar escape sequences en comillas dobles
static int	process_escape_in_double_quotes(t_expand_state *state)
{
	state->input_pos++; // Saltar '\'
	
	if (!state->input[state->input_pos])
	{
		add_char_to_result(state, '\\');
		return (1);
	}
	
	// En comillas dobles, solo ciertos caracteres pueden ser escapados
	if (state->input[state->input_pos] == '"' || 
		state->input[state->input_pos] == '\\' || 
		state->input[state->input_pos] == '$' || 
		state->input[state->input_pos] == '`' ||
		state->input[state->input_pos] == '\n')
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

// Función principal de expansión
char	*process_quotes_and_variables(char *input, char **env, int exit_status)
{
	t_expand_state	state;

	if (!input)
		return (NULL);
	
	// Inicializar estado
	state.input = input;
	state.input_pos = 0;
	state.result_size = ft_strlen(input) * 2 + 256;
	state.result = malloc(state.result_size);
	if (!state.result)
		return (NULL);
	state.result_pos = 0;
	state.in_single_quotes = 0;
	state.in_double_quotes = 0;
	state.env = env;
	state.exit_status = exit_status;
	
	while (state.input[state.input_pos])
	{
		char current = state.input[state.input_pos];
		
		if (current == '\'' && !state.in_double_quotes)
		{
			state.in_single_quotes = !state.in_single_quotes;
			state.input_pos++; // Consumir la comilla pero no añadirla al resultado
		}
		else if (current == '"' && !state.in_single_quotes)
		{
			state.in_double_quotes = !state.in_double_quotes;
			state.input_pos++; // Consumir la comilla pero no añadirla al resultado
		}
		else if (current == '$' && !state.in_single_quotes)
		{
			expand_variable_internal(&state);
		}
		else if (current == '\\' && state.in_double_quotes)
		{
			process_escape_in_double_quotes(&state);
		}
		else
		{
			add_char_to_result(&state, current);
			state.input_pos++;
		}
	}
	
	state.result[state.result_pos] = '\0';
	return (state.result);
}

// Función principal de expansión de variables (interfaz pública)
char	*expand_variable(char *str, char **env, int exit_status)
{
	return (process_quotes_and_variables(str, env, exit_status));
}

// Funciones auxiliares mantenidas para compatibilidad
int	process_env_variable(char *str, char *result, int *j, char **env)
{
	char	*expanded;
	char	*temp_input;
	int		pos = 0;
	
	temp_input = ft_strjoin("$", str);
	if (!temp_input)
		return (0);
	
	expanded = process_quotes_and_variables(temp_input, env, 0);
	free(temp_input);
	
	if (expanded)
	{
		ft_strlcpy(result + *j, expanded, 4096 - *j);
		*j += ft_strlen(expanded);
		
		// Calcular cuántos caracteres se consumieron
		while (str[pos] && (ft_isalnum(str[pos]) || str[pos] == '_'))
			pos++;
		
		free(expanded);
	}
	return (pos);
}
