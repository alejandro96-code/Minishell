/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   comillas.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 09:50:58 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/07 21:34:53 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función para encontrar el valor de una variable de entorno
static char	*get_env_value(char *var_name, char **env)
{
	int	i;
	int	len;

	if (!var_name || !env)
		return (NULL);
	len = strlen(var_name);
	i = 0;
	while (env[i])
	{
		if (strncmp(env[i], var_name, len) == 0 && env[i][len] == '=')
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

	i = 0;
	
	// Casos especiales
	if (str[*pos] == '?')
	{
		(*pos)++;
		return (strdup("0")); // Exit status, simplificado como 0
	}
	if (str[*pos] == '$')
	{
		(*pos)++;
		return (strdup("$$")); // PID del proceso
	}
	
	// Extraer nombre de variable
	while (str[*pos] && (isalnum(str[*pos]) || str[*pos] == '_') && i < 255)
	{
		var_name[i++] = str[(*pos)++];
	}
	var_name[i] = '\0';
	
	if (i == 0)
		return (strdup("$")); // $ solo, sin variable válida
	
	value = get_env_value(var_name, env);
	return (value ? strdup(value) : strdup(""));
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
				while (result_len + strlen(var_expansion) + 1 >= result_size)
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
				strcpy(result + result_len, var_expansion);
				result_len += strlen(var_expansion);
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
	
	strncpy(result, str + start, len);
	result[len] = '\0';
	
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
			while (result_len + strlen(segment) + 1 >= result_size)
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
			
			strcpy(result + result_len, segment);
			result_len += strlen(segment);
			free(segment);
		}
	}
	
	result[result_len] = '\0';
	return (result);
}

// Función de compatibilidad con el código existente
char	*input_quotes(char *input, char **env)
{
	return (process_quotes_and_variables(input, env));
}

// Función de compatibilidad con el código existente
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
		strcpy(result + *j, expanded);
		*j += strlen(expanded);
		free(expanded);
	}
	return (pos);
}

char	*quotes_clean(char *text, char quote, char **env)
{
	if (quote == '"')
	{
		int pos = 0;
		return (process_double_quotes(text - 1, &pos, env)); // -1 para simular la comilla inicial
	}
	else
	{
		int pos = 0;
		return (process_single_quotes(text - 1, &pos, env)); // -1 para simular la comilla inicial
	}
}

// Función de test mejorada
int main(int argc, char **argv, char **env)
{
	char	*test_cases[] = {
		"echo \"hola $USER\"",
		"echo 'hola $USER'",
		"echo $USER",
		"echo \"$USER está en $HOME\"",
		"echo '$USER no se expande'",
		"echo \"precio: $100\"",
		"echo \"exit code: $?\"",
		"echo \"proceso: $$\"",
		"echo \"comillas: \\\"texto\\\"\"",
		"echo $USER$HOME",
		NULL
	};
	int i = 0;
	
	(void)argc; // Evitar warning de parámetro no usado
	(void)argv; // Evitar warning de parámetro no usado
	
	printf("=== PRUEBAS DE EXPANSIÓN DE VARIABLES Y COMILLAS ===\n\n");
	
	while (test_cases[i])
	{
		printf("Input:  %s\n", test_cases[i]);
		printf("Output: %s\n", process_quotes_and_variables(test_cases[i], env));
		printf("---\n");
		i++;
	}
	
	return (0);
}