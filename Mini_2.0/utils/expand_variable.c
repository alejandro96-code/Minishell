/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/11 13:38:47 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Funcion que mantiene el bucle y delega la expansión
static int	handle_dollar_sequence(char *str, char *result, int *j, char **env)
{
	int	consumed;

	if (str[1] == '?')
	{
		result[(*j)++] = '0';
		return (2);
	}
	else if (str[1] != '\0' && str[1] != ' ')
	{
		consumed = process_env_variable(str + 1, result, j, env);
		return (consumed + 1);
	}
	else
	{
		result[(*j)++] = str[0];
		return (1);
	}
}

static void	expand_loop(char *str, char *result, char **env)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (str[i] && j < 4095)
	{
		if (str[i] == '$')
			i += handle_dollar_sequence(str + i, result, &j, env);
		else
			result[j++] = str[i++];
	}
	result[j] = '\0';
}

char	*expand_variable(char *str, char **env)
{
	char	*result;

	if (!str)
		return (NULL);
	result = malloc(4096);
	if (!result)
		return (strdup(str));
	expand_loop(str, result, env);
	return (result);
}

// Funcion que process_env_variable: se encarga de procesar la expansión de $VAR
static int	extract_var_name(char *str, char *var_name)
{
	int	name_len;

	name_len = 0;
	while (str[name_len] && (isalnum(str[name_len]) || str[name_len] == '_')
		&& name_len < 255)
	{
		var_name[name_len] = str[name_len];
		name_len++;
	}
	var_name[name_len] = '\0';
	return (name_len);
}

static char	*find_env_value(char *var_name, int name_len, char **env)
{
	int	k;

	k = 0;
	while (env[k])
	{
		if (strncmp(env[k], var_name, name_len) == 0 && env[k][name_len] == '=')
			return (env[k] + name_len + 1);
		k++;
	}
	return (NULL);
}

int	process_env_variable(char *str, char *result, int *j, char **env)
{
	char	var_name[256] = {0};
	int		name_len;
	char	*value;
	int		value_len;

	name_len = extract_var_name(str, var_name);
	if (name_len > 0)
	{
		value = find_env_value(var_name, name_len, env);
		if (value)
		{
			value_len = strlen(value);
			if (*j + value_len < 4095)
			{
				strcpy(result + *j, value);
				*j += value_len;
			}
		}
		else
			fprintf(stderr, "Warning: Variable %s not found in environment\n",
				var_name);
	}
	return (name_len);
}
