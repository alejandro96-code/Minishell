/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/15 20:23:35 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función para obtener el valor de una variable de entorno
char	*get_env_var(char *name, char **env)
{
	int		cont;
	size_t	name_len;

	cont = 0;
	name_len = strlen(name);
	while (env[cont])
	{
		if (ft_strncmp(env[cont], name, name_len) == 0
			&& env[cont][name_len] == '=')
			return (&env[cont][name_len + 1]);
		cont++;
	}
	return (NULL);
}

// Implementación del builtin cd
int	builtin_cd(char **args, char **env)
{
	char	*path;

	path = args[1];
	if (!path)
	{
		path = get_env_var("HOME", env);
		if (!path)
		{
			perror("cd: HOME not set");
			return (1);
		}
	}
	if (path[0] == '~')
		path = resolve_home_path(path, env);
	else if (path[0] != '/')
		path = resolve_relative_path(path);
	if (!path)
		return (1);
	return (try_change_directory(path));
}

char	*resolve_home_path(char *path, char **env)
{
	char	*home;
	char	*new_path;

	home = get_env_var("HOME", env);
	if (!home)
	{
		fprintf(stderr, "cd: HOME not set\n");
		return (NULL);
	}
	new_path = malloc(strlen(home) + strlen(path));
	if (!new_path)
	{
		perror("malloc");
		return (NULL);
	}
	strcpy(new_path, home);
	ft_strcat(new_path, path + 1);
	return (new_path);
}

char	*resolve_relative_path(char *path)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd");
		return (NULL);
	}
	ft_strcat(cwd, "/");
	ft_strcat(cwd, path);
	return (ft_strdup(cwd));
}

int	try_change_directory(char *path)
{
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}
