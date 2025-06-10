/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:36:06 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	find_env_var_index(char **env, char *name)
{
	int		i;
	size_t	name_len;

	i = 0;
	name_len = ft_strlen(name);
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

int	builtin_unset(char **args, char ***env)
{
	int	i;

	i = 1;
	if (!args[1])
	{
		write_error_msg("unset: not enough arguments\n");
		return (1);
	}
	while (args[i])
	{
		unset_variable(args[i], env);
		i++;
	}
	return (0);
}

void	unset_variable(char *var, char ***env)
{
	int		i;

	i = find_env_var_index(*env, var);
	if (i != -1)
		remove_env_entry(i, env);
}

void	remove_env_entry(int index, char ***env)
{
	free((*env)[index]);
	while ((*env)[index + 1])
	{
		(*env)[index] = (*env)[index + 1];
		index++;
	}
	(*env)[index] = NULL;
}
