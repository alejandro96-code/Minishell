/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 18:52:33 by alejanr2         ###   ########.fr       */
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

	status = exit_status;
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
	exit(status);
}
