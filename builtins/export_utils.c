/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 00:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/27 17:49:46 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*find_equal_in_string(char *str)
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

int	check_env_key_match(char *env_str, char *key, size_t key_len)
{
	int	j;

	j = 0;
	while (env_str[j] && env_str[j] != '=')
		j++;
	if (env_str[j] == '=' && (size_t)j == key_len
		&& ft_strncmp(env_str, key, key_len) == 0)
		return (1);
	if (!env_str[j] && ft_strncmp(env_str, key, key_len) == 0
		&& env_str[key_len] == '\0')
		return (1);
	return (0);
}

int	replace_env_at_index(int i, char *new_value, char ***env)
{
	char	*new_str;

	new_str = ft_strdup(new_value);
	if (!new_str)
		return (0);
	free((*env)[i]);
	(*env)[i] = new_str;
	return (1);
}

int	replace_env_value(char *key, char *new_value, char ***env)
{
	int		i;
	size_t	key_len;
	char	*env_str;

	if (!key || !new_value || !env || !*env)
		return (0);
	i = 0;
	key_len = ft_strlen(key);
	while ((*env)[i])
	{
		env_str = (*env)[i];
		if (check_env_key_match(env_str, key, key_len))
			return (replace_env_at_index(i, new_value, env));
		i++;
	}
	return (0);
}

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
			if ((size_t)(env_equal - env[i]) == key_len && ft_strncmp(env[i],
					key, key_len) == 0)
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
