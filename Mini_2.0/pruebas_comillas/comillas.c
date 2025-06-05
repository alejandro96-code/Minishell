/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   comillas.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 09:50:58 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/04 18:42:15 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

char	*quotes_clean(char *text, char quote, char **env)
{
	int		i;
	char	*var_env;
	char	*text_clean;

	text_clean = ft_calloc(1, sizeof(char));
	i = 0;
	if (quote == '"')
	{
		while (text[i])
		{
			if (text[i] == '$')
			{
				var_env = ft_calloc(1, sizeof(char));
				while (text[i] && (isalnum(text[i]) || text[i] == '_'))
				{
					var_env = ft_strjoin_char(var_env, text[i++]);
				}
				var_env = expand_variable(var_env, env);
				text_clean = ft_strjoin_s1_free(text_clean, var_env);
				free(var_env);
			}
			else
				text_clean = ft_strjoin_char(text_clean, text[i]);
			i++;
		}
	}
	else
	{
		while (text[i])
		{
			text_clean = ft_strjoin_char(text_clean, text[i]);
			i++;
		}
	}
	return (text_clean);
}

char	*input_quotes(char *input, char **env)
{
	int		i;
	char	*text_whit_quotes;
	char	quotes;
	char	*input_clean;

	input_clean = ft_calloc(1, sizeof(char));
	text_whit_quotes = ft_calloc(1, sizeof(char));
	i = 0;
	while (input[i])
	{
		if (input[i] == '"' || input[i] == '\'')
		{
			quotes = input[i];
			while (input[i + 1] == quotes)
			{
				text_whit_quotes = ft_strjoin_char(text_whit_quotes, input[i + 1]);
				i++;
			}
			input_clean = quotes_clean(text_whit_quotes, quotes, env);
			free(text_whit_quotes);
		}
		else
			input_clean = ft_strjoin_char(input_clean, input[i]);
		i++;
	}
	return (input_clean);
}


int main (int argv ,char **argc, char **env)
{
	printf("%s", input_quotes("echo \"hola $USER\"", env));
}