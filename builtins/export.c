/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 18:51:52 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	print_export_vars(char **env)
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

int	builtin_export(char **args, char ***env)
{
	int	i;

	if (!args[1])
	{
		print_export_vars(*env);
		return (0);
	}
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
			ft_putstr_fd("export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			return (1);
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
	int		i;

	if (!arg)
		return (0);
	arg_copy = ft_strdup(arg);
	if (!arg_copy)
		return (0);
	
	// Buscar el signo igual manualmente
	equal = NULL;
	i = 0;
	while (arg_copy[i])
	{
		if (arg_copy[i] == '=')
		{
			equal = &arg_copy[i];
			break;
		}
		i++;
	}
	
	if (!equal)
	{
		// Variable sin valor - solo verificar si ya existe
		if (find_env_var(arg_copy, *env) != -1)
		{
			free(arg_copy);
			return (1);  // Ya existe, no hacer nada
		}
		// Agregar variable sin valor
		result = add_env_value(arg, env);
		free(arg_copy);
		return (result);
	}
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
	char	*new_str;

	if (!key || !new_value || !env || !*env)
		return (0);
	i = 0;
	key_len = ft_strlen(key);
	while ((*env)[i])
	{
		env_equal = ft_strchr((*env)[i], '=');
		if (env_equal && (size_t)(env_equal - (*env)[i]) == key_len
			&& ft_strncmp((*env)[i], key, key_len) == 0)
		{
			new_str = ft_strdup(new_value);
			if (!new_str)
				return (0);
			free((*env)[i]);
			(*env)[i] = new_str;
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

	if (!new_value || !env || !*env)
		return (0);
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
			if (ft_strncmp(env[i], key, key_len) == 0 && env[i][key_len] == '\0')
				return (i);
		}
		i++;
	}
	return (-1);
}
