/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:36:07 by dgasco-g         ###   ########.fr       */
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
		return (ft_strdup(str));
	expand_loop(str, result, env);
	return (result);
}

// Funcion que process_env_variable: se encarga de procesar la expansión de $VAR
static int	extract_var_name(char *str, char *var_name)
{
	int	name_len;

	name_len = 0;
	while (str[name_len] && (ft_isalnum(str[name_len]) || str[name_len] == '_')
		&& name_len < 255)
	{
		var_name[name_len] = str[name_len];
		name_len++;
	}
	var_name[name_len] = '\0';
	return (name_len);
}

char	*get_env_value(char *var_name, char **env)
{
	int		i;
	size_t	name_len;

	i = 0;
	name_len = ft_strlen(var_name);
	while (env[i])
	{
		if (ft_strncmp(env[i], var_name, name_len) == 0 && env[i][name_len] == '=')
			return (&env[i][name_len + 1]);
		i++;
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
		value = get_env_value(var_name, env);
		if (value)
		{
			value_len = ft_strlen(value);
			if (*j + value_len < 4095)
			{
				ft_strlcpy(result + *j, value, value_len + 1);
				*j += value_len;
			}
		}
		else
		{
			write(STDERR_FILENO, "Warning: Variable ", 18);
			write(STDERR_FILENO, var_name, ft_strlen(var_name));
			write(STDERR_FILENO, " not found in environment\n", 26);
		}
	}
	return (name_len);
}
