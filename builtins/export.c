/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 21:43:40 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	return (str[cont] == '=');
}

int	builtin_export(char **args, char ***env)
{
	int	i;

	i = 1;
	while (args[i])
	{
		if (is_valid_export(args[i]))
		{
			if (!handle_export(args[i], env))
				return (1);
		}
		else
		{
			write_export_error(args[i]);
		}
		i++;
	}
	return (0);
}

int	handle_export(char *arg, char ***env)
{
	char	*arg_copy;
	char	*equal;
	char	*key;
	int		result;

	arg_copy = ft_strdup(arg);
	if (!arg_copy)
		return (0);
	equal = ft_strchr(arg_copy, '=');
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

int	replace_env_value(char *key, char *new_value, char ***env)
{
	int		i;
	size_t	key_len;
	char	*env_equal;

	i = 0;
	key_len = ft_strlen(key);
	while ((*env)[i])
	{
		env_equal = ft_strchr((*env)[i], '=');
		if (env_equal && (size_t)(env_equal - (*env)[i]) == key_len
			&& ft_strncmp((*env)[i], key, key_len) == 0)
		{
			free((*env)[i]);
			(*env)[i] = ft_strdup(new_value);
			return (1);
		}
		i++;
	}
	return (0);
}

int	add_env_value(char *new_value, char ***env)
{
	int		len;
	char	**new_env;
	int		i;

	len = 0;
	while ((*env)[len])
		len++;
	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
	{
		perror("malloc");
		return (0);
	}
	i = 0;
	while (i < len)
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[len] = ft_strdup(new_value);
	new_env[len + 1] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}
