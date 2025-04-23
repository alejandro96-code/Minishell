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
	while (*str && isspace(*str))
		str++;
	if (*str == '+' || *str == '-')
		str++;
	if (!*str)
		return (0);
	while (*str)
	{
		if (!isdigit(*str))
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
			fprintf(stderr, "exit: %s: numeric argument required\n", args[1]);
			exit(255);
		}
		status = atoi(args[1]);
		if (args[2])
		{
			fprintf(stderr, "exit: too many arguments\n");
			return (1);
		}
	}
	printf("exit\n");
	exit(status);
}
