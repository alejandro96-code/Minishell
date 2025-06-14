/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 17:46:19 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/14 13:03:38 by alejandro        ###   ########.fr       */
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

// Alias para mantener compatibilidad con main.c
char	**copy_env(char **env)
{
	return (copy_env_array(env));
}
