/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/15 20:24:43 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_echo(char **args, char **env)
{
	int		cont;
	int		print_newline;
	char	*expanded_str;

	cont = 1;
	print_newline = 1;
	if (args[1] && strcmp(args[1], "-n") == 0)
	{
		print_newline = 0;
		cont = 2;
	}
	while (args[cont])
	{
		expanded_str = expand_variable(args[cont], env);
		printf("%s", expanded_str);
		free(expanded_str);
		if (args[cont + 1])
			printf(" ");
		cont++;
	}
	if (print_newline)
		printf("\n");
	return (0);
}
