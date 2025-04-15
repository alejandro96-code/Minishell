/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/15 20:42:54 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_unset(char **args, char ***env)
{
	int	i;

	i = 1;
	if (!args[1])
	{
		fprintf(stderr, "unset: not enough arguments\n");
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
	size_t	len;
	int		i;

	len = strlen(var);
	i = 0;
	while ((*env)[i])
	{
		if (strncmp((*env)[i], var, len) == 0 && (*env)[i][len] == '=')
		{
			remove_env_entry(i, env);
			continue ;
		}
		i++;
	}
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
