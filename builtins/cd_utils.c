/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 04:15:00 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/29 04:19:20 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Maneja el caso cuando no se proporciona path (ir a HOME)
char	*handle_no_path_case(char **env, int *should_free)
{
	char	*path;

	path = get_env_var("HOME", env);
	if (!path)
	{
		ft_putstr_fd("cd: HOME not set\n", 2);
		return (NULL);
	}
	*should_free = 0;
	return (path);
}

// Maneja el caso de path con tilde (~)
char	*handle_tilde_path(char *original_path, char **env, int *should_free)
{
	char	*allocated_path;

	allocated_path = resolve_home_path(original_path, env);
	if (!allocated_path)
		return (NULL);
	*should_free = 1;
	return (allocated_path);
}

// Maneja el caso de path relativo
char	*handle_relative_path_case(char *original_path, int *should_free)
{
	char	*allocated_path;

	allocated_path = resolve_relative_path(original_path);
	if (!allocated_path)
		return (NULL);
	*should_free = 1;
	return (allocated_path);
}

char	*resolve_home_path(char *path, char **env)
{
	char	*home;
	char	*new_path;

	home = get_env_var("HOME", env);
	if (!home)
	{
		ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (NULL);
	}
	new_path = malloc(ft_strlen(home) + ft_strlen(path));
	if (!new_path)
	{
		perror("malloc");
		return (NULL);
	}
	ft_strlcpy(new_path, home, ft_strlen(home) + 1);
	ft_strcat(new_path, path + 1);
	return (new_path);
}

char	*resolve_relative_path(char *path)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return (NULL);
	}
	ft_strcat(cwd, "/");
	ft_strcat(cwd, path);
	return (ft_strdup(cwd));
}
