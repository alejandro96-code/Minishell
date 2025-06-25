/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_array_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 13:55:03 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/14 13:55:03 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Cuenta las variables de entorno
int	count_env_vars(char **env)
{
	int	count;

	count = 0;
	while (env[count])
		count++;
	return (count);
}

// Copia las variables de entorno
char	**copy_env_array(char **env)
{
	char	**new_env;
	int		count;
	int		i;

	count = count_env_vars(env);
	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_env[i] = ft_strdup(env[i]);
		if (!new_env[i])
		{
			free_env(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

static char	**create_new_env_array(int len)
{
	char	**new_env;

	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
	{
		perror("malloc");
		return (NULL);
	}
	return (new_env);
}

static int	copy_existing_env(char **new_env, char **env, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		new_env[i] = env[i];
		i++;
	}
	return (1);
}

int	add_env_value(char *new_value, char ***env)
{
	int		len;
	char	**new_env;

	if (!new_value || !env || !*env)
		return (0);
	len = 0;
	while ((*env)[len])
		len++;
	new_env = create_new_env_array(len);
	if (!new_env)
		return (0);
	copy_existing_env(new_env, *env, len);
	new_env[len] = ft_strdup(new_value);
	if (!new_env[len])
	{
		free(new_env);
		return (0);
	}
	new_env[len + 1] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}
