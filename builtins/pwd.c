/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 18:52:00 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Verificar si una opción es válida
static int	is_valid_option(const char *arg)
{
	if (arg[0] != '-' || ft_strlen(arg) != 2)
		return (0);
	return (arg[1] == 'L' || arg[1] == 'P');
}

// Mostrar error de opción inválida
static int	show_pwd_error(const char *option)
{
	ft_putstr_fd("pwd: invalid option -- '", STDERR_FILENO);
	ft_putchar_fd(option[1], STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	return (1);
}

int	builtin_pwd(char **args, char **env)
{
	char	cwd[4096];
	int		i;

	i = 1;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			if (!is_valid_option(args[i]))
				return (show_pwd_error(args[i]));
		}
		i++;
	}
	if (getcwd(cwd, sizeof(cwd)))
		printf("%s\n", cwd);
	else
		perror("pwd");
	(void)env;
	return (0);
}
