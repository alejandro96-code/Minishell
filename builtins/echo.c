/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/15 13:34:03 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	output_char_sequence(const char *sequence, size_t len)
{
	ssize_t	result;

	result = write(1, sequence, len);
	(void)result;
}

static int	parse_echo_options(char *arg, int *newline, int *interpret_escapes)
{
	int	i;

	if (arg && ft_strncmp(arg, "-e", 3) == 0)
	{
		*interpret_escapes = 1;
		return (1);
	}
	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 2;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	*newline = 0;
	return (1);
}

static void	print_basic_escapes(char c)
{
	if (c == 'n')
		output_char_sequence("\n", 1);
	else if (c == 't')
		output_char_sequence("\t", 1);
	else if (c == 'r')
		output_char_sequence("\r", 1);
	else if (c == 'b')
		output_char_sequence("\b", 1);
	else if (c == 'a')
		output_char_sequence("\a", 1);
	else if (c == 'v')
		output_char_sequence("\v", 1);
	else if (c == 'f')
		output_char_sequence("\f", 1);
	else if (c == '\\')
		output_char_sequence("\\", 1);
	else if (c == '0')
		output_char_sequence("\0", 1);
	else
	{
		output_char_sequence("\\", 1);
		output_char_sequence(&c, 1);
	}
}

static void	print_with_escapes(char *str)
{
	while (*str)
	{
		if (*str == '\\' && *(str + 1))
		{
			str++;
			print_basic_escapes(*str);
		}
		else
			output_char_sequence(str, 1);
		str++;
	}
}

int	builtin_echo(char **args, char **env)
{
	int	i;
	int	newline;
	int	interpret_escapes;

	(void)env;
	i = 1;
	newline = 1;
	interpret_escapes = 0;
	while (args[i] && parse_echo_options(args[i], &newline, &interpret_escapes))
		i++;
	while (args[i])
	{
		if (interpret_escapes)
			print_with_escapes(args[i]);
		else
			output_char_sequence(args[i], ft_strlen(args[i]));
		if (args[i + 1])
			output_char_sequence(" ", 1);
		i++;
	}
	if (newline)
		output_char_sequence("\n", 1);
	return (0);
}
