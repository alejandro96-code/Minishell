/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 13:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/23 14:45:00 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Verifica si el comando es un builtin
int	is_builtin_command(const char *cmd)
{
	if (!cmd)
		return (0);
	return ((ft_strncmp(cmd, "echo", 4) == 0 && ft_strlen(cmd) == 4)
		|| (ft_strncmp(cmd, "cd", 2) == 0 && ft_strlen(cmd) == 2)
		|| (ft_strncmp(cmd, "pwd", 3) == 0 && ft_strlen(cmd) == 3)
		|| (ft_strncmp(cmd, "export", 6) == 0 && ft_strlen(cmd) == 6)
		|| (ft_strncmp(cmd, "unset", 5) == 0 && ft_strlen(cmd) == 5)
		|| (ft_strncmp(cmd, "env", 3) == 0 && ft_strlen(cmd) == 3)
		|| (ft_strncmp(cmd, "exit", 4) == 0 && ft_strlen(cmd) == 4));
}

int	is_redirection(char c)
{
	if (c == '<' || c == '>')
		return (1);
	return (0);
}

char	*aux_split_redirection(char **str, char *start)
{
	char	redirect;

	redirect = **str;
	(*str)++;
	if (**str && is_redirection(**str) && **str == redirect)
	{
		(*str)++;
		return (ft_substr(start, 0, 2));
	}
	return (ft_substr(start, 0, 1));
}

int	redirection_last_arg(const char *input)
{
	int	i;
	int	boolean;

	boolean = 0;
	i = 0;
	while (input[i])
	{
		if (!boolean && (is_redirection(input[i])))
			boolean = 1;
		else if (boolean && (!is_redirection(input[i]))
			&& !ft_isspace(input[i]))
			boolean = 0;
		i++;
	}
	if (boolean)
		return (0);
	return (1);
}

// Valida la sintaxis de las redirecciones
int	validate_redirections(const char *input)
{
	int	i;
	int	in_single_quotes;
	int	in_double_quotes;

	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (input[i])
	{
		update_quote_state(input[i], &in_single_quotes, &in_double_quotes);
		if (!in_single_quotes && !in_double_quotes && is_redirection(input[i]))
		{
			if (input[i + 1] && input[i] == input[i + 1])
				i++;
			i++;
			while (input[i] && (input[i] == ' ' || input[i] == '\t'))
				i++;
			if (input[i] && is_redirection(input[i--]))
				return (0);
		}
		i++;
	}
	return (redirection_last_arg(input));
}

// Verifica si un argumento es un operador de redirección
static int	is_redirection_operator(char *arg)
{
	return ((ft_strncmp(arg, "<", 1) == 0 && ft_strlen(arg) == 1)
		|| (ft_strncmp(arg, "<<", 2) == 0 && ft_strlen(arg) == 2)
		|| (ft_strncmp(arg, ">", 1) == 0 && ft_strlen(arg) == 1)
		|| (ft_strncmp(arg, ">>", 2) == 0 && ft_strlen(arg) == 2));
}

// Cuenta el número total de argumentos en el array
static int	count_args(char **args)
{
	int	count;

	count = 0;
	while (args[count])
		count++;
	return (count);
}

// Busca la posición del primer comando (no redirección) en el array
static int	find_command_position(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		if (!is_redirection_operator(args[i]))
			return (i);
		// Si es una redirección, saltamos también el archivo/delimitador
		if (args[i + 1])
			i += 2;
		else
			i++;
	}
	return (-1);
}

// Reorganiza los argumentos moviendo el comando al principio
char	**reorganize_command_args(char **args)
{
	char	**new_args;
	int		total_args;
	int		cmd_pos;
	int		i;
	int		j;

	if (!args || !args[0] || !is_redirection_operator(args[0]))
		return (args);
	cmd_pos = find_command_position(args);
	if (cmd_pos == -1)
		return (args);
	total_args = count_args(args);
	new_args = malloc(sizeof(char *) * (total_args + 1));
	if (!new_args)
		return (args);
	new_args[0] = ft_strdup(args[cmd_pos]);
	j = 1;
	i = 0;
	while (i < total_args)
	{
		if (i != cmd_pos)
			new_args[j++] = ft_strdup(args[i]);
		i++;
	}
	new_args[j] = NULL;
	return (ft_free_split(args), new_args);
}
