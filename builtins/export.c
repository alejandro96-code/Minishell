/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/26 21:17:58 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	print_export_vars(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env[i], STDOUT_FILENO);
		ft_putstr_fd("\n", STDOUT_FILENO);
		i++;
	}
	return (0);
}

static int	is_valid_export(char *str)
{
	int	cont;

	cont = 0;
	if (!str || !str[0] || !(ft_isalpha(str[0]) || str[0] == '_'))
		return (0);
	while (str[cont] && str[cont] != '=')
	{
		if (!(ft_isalnum(str[cont]) || str[cont] == '_'))
			return (0);
		cont++;
	}
	return (1);
}

static int	process_export_arg(char *arg, char ***env)
{
	if (is_valid_export(arg))
	{
		if (!handle_export(arg, env))
			return (1);
	}
	else
	{
		ft_putstr_fd("export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

int	builtin_export(char **args, char ***env)
{
	int	i;
	int	exit_code;

	if (!args[1])
		return (print_export_vars(*env));
	i = 1;
	exit_code = 0;
	while (args[i])
	{
		if (process_export_arg(args[i], env))
			exit_code = 1;
		i++;
	}
	return (exit_code);
}

int	handle_export(char *arg, char ***env)
{
	char	*arg_copy;
	char	*equal;

	if (!arg)
		return (0);
	arg_copy = ft_strdup(arg);
	if (!arg_copy)
		return (0);
	equal = find_equal_in_string(arg_copy);
	if (!equal)
		return (handle_export_no_equal(arg, arg_copy, env));
	else
		return (handle_export_with_equal(arg, arg_copy, equal, env));
}
