/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 12:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/14 12:40:17 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*build_full_path(char *dir, char *cmd)
{
	char	*temp;
	char	*full_path;

	temp = ft_strjoin(dir, "/");
	full_path = ft_strjoin(temp, cmd);
	free(temp);
	return (full_path);
}

static int	is_valid_command(char *path)
{
	return (access(path, F_OK | X_OK) == 0);
}

char	*search_in_paths(char **paths, char *cmd)
{
	int		i;
	char	*cmd_path;

	if (!paths || !cmd)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		cmd_path = build_full_path(paths[i], cmd);
		if (cmd_path && is_valid_command(cmd_path))
		{
			ft_free_split(paths);
			return (cmd_path);
		}
		free(cmd_path);
		i++;
	}
	ft_free_split(paths);
	return (NULL);
}

static int	is_absolute_path(char *cmd)
{
	return (cmd[0] == '/' || cmd[0] == '.');
}

char	*find_command_path(char *cmd, char **env)
{
	char	*path_env;
	char	**paths;

	if (is_absolute_path(cmd))
		return (ft_strdup(cmd));
	path_env = get_env_var("PATH", env);
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_paths(paths, cmd));
}
