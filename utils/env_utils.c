/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:30:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_env_vars(char **envp)
{
	int	count;

	count = 0;
	while (envp[count])
		count++;
	return (count);
}

static void	copy_env_vars(char **envp, char **copy)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[i] = NULL;
}

char	**copy_env(char **envp)
{
	int		count;
	char	**copy;

	count = count_env_vars(envp);
	copy = malloc((count + 1) * sizeof(char *));
	if (!copy)
		return (NULL);
	copy_env_vars(envp, copy);
	return (copy);
}

void	free_env(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}
