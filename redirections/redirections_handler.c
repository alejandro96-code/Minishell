/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_handler.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:00:00 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/19 01:56:07 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Procesa una redirección individual según el tipo
static void	handle_redirection_type(char **args, int *i, char *cleaned_filename,
		char **env)
{
	char	*cleaned_delimiter;
	int		result;

	if (ft_strncmp(args[*i], "<", 1) == 0 && ft_strlen(args[*i]) == 1)
	{
		result = redirect_input(cleaned_filename);
		if (result != 0)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cleaned_filename, STDERR_FILENO);
			ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		}
	}
	else if (ft_strncmp(args[*i], "<<", 2) == 0 && ft_strlen(args[*i]) == 2)
	{
		cleaned_delimiter = remove_quotes(args[*i + 1]);
		heredoc(cleaned_delimiter, env);
		if (cleaned_delimiter != args[*i + 1])
			free(cleaned_delimiter);
	}
	else if (ft_strncmp(args[*i], ">", 1) == 0 && ft_strlen(args[*i]) == 1)
		redirect_output(cleaned_filename, 0);
	else if (ft_strncmp(args[*i], ">>", 2) == 0 && ft_strlen(args[*i]) == 2)
		redirect_output(cleaned_filename, 1);
	(*i)++;
}

// Procesa una redirección completa con expansión y limpieza
static void	process_redirection(char **args, int *i, char **env)
{
	char	*expanded_filename;
	char	*cleaned_filename;

	if (!args[*i + 1])
		return ;
	expanded_filename = expand_variable(args[*i + 1], env, 0);
	cleaned_filename = remove_quotes(expanded_filename);
	handle_redirection_type(args, i, cleaned_filename, env);
	if (expanded_filename != args[*i + 1])
		free(expanded_filename);
	if (cleaned_filename != expanded_filename)
		free(cleaned_filename);
}

// Verifica si un argumento es una redirección
static int	is_redirection_operator(char *arg)
{
	return ((ft_strncmp(arg, "<", 1) == 0 && ft_strlen(arg) == 1)
		|| (ft_strncmp(arg, "<<", 2) == 0 && ft_strlen(arg) == 2)
		|| (ft_strncmp(arg, ">", 1) == 0 && ft_strlen(arg) == 1)
		|| (ft_strncmp(arg, ">>", 2) == 0 && ft_strlen(arg) == 2));
}

// Procesa un operador de redirección y actualiza el índice
static int	process_redirection_operator(char **args, int *i, int count,
		char **env)
{
	if (*i + 1 >= count || !args[*i + 1])
		return (0);
	process_redirection(args, i, env);
	*i += 1; // Solo incrementar en 1, ya que process_redirection ya incrementa el índice
	return (1);
}

// Crea un nuevo array de argumentos sin las redirecciones
char	**create_filtered_args(char **args, int count, char **env)
{
	char	**new_args;
	int		i;
	int		j;

	new_args = malloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return (NULL);
	i = 0;
	j = 0;
	while (i < count && args[i])
	{
		if (is_redirection_operator(args[i]))
		{
			if (!process_redirection_operator(args, &i, count, env))
			{
				free(new_args);
				return (NULL);
			}
			continue ;
		}
		new_args[j++] = ft_strdup(args[i]);
		i++;
	}
	new_args[j] = NULL;
	return (new_args);
}
