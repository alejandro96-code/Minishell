/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/15 20:43:04 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_numeric(char *str)
{
	if (!str)
		return (0);
	while (*str && ft_isspace(*str))
		str++;
	if (*str == '+' || *str == '-')
		str++;
	if (!*str)
		return (0);
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		str++;
	}
	return (1);
}

int	builtin_exit(char **args)
{
	int	status;

	status = 0;
	if (args[1])
	{
		if (!is_numeric(args[1]))
		{
			write(STDERR_FILENO, "exit: ", 6);
			write(STDERR_FILENO, args[1], ft_strlen(args[1]));
			write(STDERR_FILENO, ": numeric argument required\n", 28);
			exit(255);
		}
		status = ft_atoi(args[1]);
		if (args[2])
		{
			write(STDERR_FILENO, "exit: too many arguments\n", 25);
			return (1);
		}
	}
	printf("exit\n");
	exit(status);
}
