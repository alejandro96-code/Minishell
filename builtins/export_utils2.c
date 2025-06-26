/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 00:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/26 21:17:57 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	copy_env_entries(char **new_env, char **env, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		new_env[i] = env[i];
		i++;
	}
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
	copy_env_entries(new_env, *env, len);
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

int	handle_export_no_equal(char *arg, char *arg_copy, char ***env)
{
	int	result;

	if (replace_env_value(arg_copy, arg, env))
	{
		free(arg_copy);
		return (1);
	}
	result = add_env_value(arg, env);
	free(arg_copy);
	return (result);
}

int	handle_export_with_equal(char *arg, char *arg_copy, char *equal,
		char ***env)
{
	char	*key;
	int		result;

	*equal = '\0';
	key = arg_copy;
	if (replace_env_value(key, arg, env))
	{
		free(arg_copy);
		return (1);
	}
	result = add_env_value(arg, env);
	free(arg_copy);
	return (result);
}
