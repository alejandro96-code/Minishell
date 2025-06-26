/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_search_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 13:55:03 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/26 13:41:23 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	find_env_var(char *key, char **env)
{
	int		i;
	size_t	key_len;
	char	*env_equal;

	i = 0;
	key_len = ft_strlen(key);
	while (env[i])
	{
		env_equal = ft_strchr(env[i], '=');
		if (env_equal)
		{
			if ((size_t)(env_equal - env[i]) == key_len
				&& ft_strncmp(env[i], key, key_len) == 0)
				return (i);
		}
		else
		{
			if (ft_strncmp(env[i], key, key_len) == 0
				&& env[i][key_len] == '\0')
				return (i);
		}
		i++;
	}
	return (-1);
}

char	*find_equal_sign(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '=')
			return (&str[i]);
		i++;
	}
	return (NULL);
}
