/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 19:23:08 by dgasco-g         ###   ########.fr       */
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

int	builtin_exit(char **args, int exit_status)
{
	int	status;

	status = exit_status; // Usar el exit status actual por defecto
	if (args[1])
	{
		if (!is_numeric(args[1]))
		{
			write_exit_error(args[1]);
			exit(255);
		}
		status = ft_atoi(args[1]);
		if (args[2])
		{
			write_error_msg("exit: too many arguments\n");
			return (1);
		}
	}
	printf("exit\n");
	exit(status);
}
