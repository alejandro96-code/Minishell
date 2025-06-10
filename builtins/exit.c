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

	status = g_exit_status; // Usar el exit status actual por defecto
	if (args[1])
	{
		if (!is_numeric(args[1]))
		{
			write_exit_error(args[1]);
			g_exit_status = 255;
			exit(255);
		}
		status = ft_atoi(args[1]);
		if (args[2])
		{
			write_error_msg("exit: too many arguments\n");
			g_exit_status = 1;
			return (1);
		}
	}
	printf("exit\n");
	g_exit_status = status;
	exit(status);
}
