/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 21:30:00 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Variable global para el exit status
extern int g_exit_status;

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

// Función para expandir una variable $VAR
static char	*expand_single_variable(char *str, int *pos, char **env)
{
	char	var_name[256];
	int		i;
	char	*value;
	char	*result;

	i = 0;
	
	// Casos especiales
	if (str[*pos] == '?')
	{
		(*pos)++;
		result = ft_itoa(g_exit_status);
		return (result ? result : ft_strdup("0"));
	}
	if (str[*pos] == '$')
	{
		(*pos)++;
		result = ft_itoa(getpid());
		return (result ? result : ft_strdup("$$"));
	}
	
	// Extraer nombre de variable
	while (str[*pos] && (ft_isalnum(str[*pos]) || str[*pos] == '_') && i < 255)
	{
		var_name[i++] = str[(*pos)++];
	}
	var_name[i] = '\0';
	
	if (i == 0)
		return (ft_strdup("$")); // $ solo, sin variable válida
	
	value = get_env_value(var_name, env);
	return (value ? ft_strdup(value) : ft_strdup(""));
}

// Función para procesar contenido dentro de comillas dobles
static char	*process_double_quotes(char *str, int *pos, char **env)
{
	char	*result;
	char	*temp;
	char	*var_expansion;
	size_t	result_size;
	size_t	result_len;
	
	result_size = 1024;
	result = malloc(result_size);
	if (!result)
		return (NULL);
	result[0] = '\0';
	result_len = 0;
	
	(*pos)++; // Saltar la comilla doble inicial
	
	while (str[*pos] && str[*pos] != '"')
	{
		if (str[*pos] == '$')
		{
			(*pos)++; // Saltar el $
			var_expansion = expand_single_variable(str, pos, env);
			if (var_expansion)
			{
				// Redimensionar si es necesario
				while (result_len + ft_strlen(var_expansion) + 1 >= result_size)
				{
					result_size *= 2;
					temp = realloc(result, result_size);
					if (!temp)
					{
						free(result);
						free(var_expansion);
						return (NULL);
					}
					result = temp;
				}
				ft_strlcpy(result + result_len, var_expansion, result_size - result_len);
				result_len += ft_strlen(var_expansion);
				free(var_expansion);
			}
		}
		else if (str[*pos] == '\\' && str[*pos + 1])
		{
			// Manejar escapes básicos dentro de comillas dobles
			(*pos)++; // Saltar la barra invertida
			if (str[*pos] == '"' || str[*pos] == '$' || str[*pos] == '\\' || str[*pos] == '\n')
			{
				result[result_len++] = str[(*pos)++];
			}
			else
			{
				result[result_len++] = '\\';
				result[result_len++] = str[(*pos)++];
			}
		}
		else
		{
			result[result_len++] = str[(*pos)++];
		}
		
		// Redimensionar si es necesario
		if (result_len + 1 >= result_size)
		{
			result_size *= 2;
			temp = realloc(result, result_size);
			if (!temp)
			{
				free(result);
				return (NULL);
			}
			result = temp;
		}
	}
	
	if (str[*pos] == '"')
		(*pos)++; // Saltar la comilla doble final
	
	result[result_len] = '\0';
	return (result);
}

// Función para procesar contenido dentro de comillas simples (literal)
static char	*process_single_quotes(char *str, int *pos, char **env)
{
	char	*result;
	int		start;
	int		len;
	
	(void)env; // No se usan en comillas simples
	
	(*pos)++; // Saltar la comilla simple inicial
	start = *pos;
	
	// Encontrar la comilla simple de cierre
	while (str[*pos] && str[*pos] != '\'')
		(*pos)++;
	
	len = *pos - start;
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	
	ft_strlcpy(result, str + start, len + 1);
	
	if (str[*pos] == '\'')
		(*pos)++; // Saltar la comilla simple final
	
	return (result);
}

// Función principal para procesar input con comillas y expansión de variables
char	*process_quotes_and_variables(char *input, char **env)
{
	char	*result;
	char	*temp;
	char	*segment;
	size_t	result_size;
	size_t	result_len;
	int		pos;
	
	if (!input)
		return (NULL);
	
	result_size = 1024;
	result = malloc(result_size);
	if (!result)
		return (NULL);
	result[0] = '\0';
	result_len = 0;
	pos = 0;
	
	while (input[pos])
	{
		if (input[pos] == '"')
		{
			// Procesar comillas dobles (con expansión)
			segment = process_double_quotes(input, &pos, env);
		}
		else if (input[pos] == '\'')
		{
			// Procesar comillas simples (literal)
			segment = process_single_quotes(input, &pos, env);
		}
		else if (input[pos] == '$')
		{
			// Expansión de variable fuera de comillas
			pos++; // Saltar el $
			segment = expand_single_variable(input, &pos, env);
		}
		else
		{
			// Carácter normal
			segment = malloc(2);
			if (!segment)
			{
				free(result);
				return (NULL);
			}
			segment[0] = input[pos++];
			segment[1] = '\0';
		}
		
		if (segment)
		{
			// Redimensionar si es necesario
			while (result_len + ft_strlen(segment) + 1 >= result_size)
			{
				result_size *= 2;
				temp = realloc(result, result_size);
				if (!temp)
				{
					free(result);
					free(segment);
					return (NULL);
				}
				result = temp;
			}
			
			ft_strlcpy(result + result_len, segment, result_size - result_len);
			result_len += ft_strlen(segment);
			free(segment);
		}
	}
	
	result[result_len] = '\0';
	return (result);
}

// Función principal de expansión de variables (interfaz pública)
char	*expand_variable(char *str, char **env)
{
	return (process_quotes_and_variables(str, env));
}

// Funciones auxiliares mantenidas para compatibilidad
int	process_env_variable(char *str, char *result, int *j, char **env)
{
	char	*expanded;
	int		pos = 0;
	
	expanded = expand_single_variable(str, &pos, env);
	if (expanded)
	{
		ft_strlcpy(result + *j, expanded, 4096 - *j);
		*j += ft_strlen(expanded);
		free(expanded);
	}
	return (pos);
}
