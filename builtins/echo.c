/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 12:54:57 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	safe_write(int fd, const void *buf, size_t count)
{
	ssize_t	result;

	result = write(fd, buf, count);
	(void)result;
}

static int	is_valid_n_option(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 2;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static void	print_basic_escapes(char c)
{
	if (c == 'n')
		safe_write(1, "\n", 1);
	else if (c == 't')
		safe_write(1, "\t", 1);
	else if (c == 'r')
		safe_write(1, "\r", 1);
	else if (c == 'b')
		safe_write(1, "\b", 1);
	else if (c == 'a')
		safe_write(1, "\a", 1);
	else if (c == 'v')
		safe_write(1, "\v", 1);
	else if (c == 'f')
		safe_write(1, "\f", 1);
	else if (c == '\\')
		safe_write(1, "\\", 1);
	else if (c == '0')
		safe_write(1, "\0", 1);
	else
	{
		safe_write(1, "\\", 1);
		safe_write(1, &c, 1);
	}
}

static void	print_escape_sequence(char **str)
{
	char	*s;

	s = *str;
	s++;
	print_basic_escapes(*s);
	*str = s;
}

static void	print_with_escapes(char *str)
{
	while (*str)
	{
		if (*str == '\\' && *(str + 1))
			print_escape_sequence(&str);
		else
			safe_write(1, str, 1);
		str++;
	}
}

int	builtin_echo(char **args, char **env)
{
	int		i;
	int		newline;

	(void)env;
	i = 1;
	newline = 1;
	while (args[i] && is_valid_n_option(args[i]))
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		print_with_escapes(args[i]);
		if (args[i + 1])
			safe_write(1, " ", 1);
		i++;
	}
	if (newline)
		safe_write(1, "\n", 1);
	return (0);
}
