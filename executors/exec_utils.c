/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 12:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/14 12:40:17 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	**extract_paths(char **envp)
{
	int		i;
	char	*path_var;

	i = 0;
	path_var = NULL;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path_var = envp[i] + 5;
			return (ft_split(path_var, ':'));
		}
		i++;
	}
	return (NULL);
}

static int	validate_command_path(char *cmd_path)
{
	if (!cmd_path)
		return (0);
	if (access(cmd_path, X_OK) == 0)
		return (1);
	return (0);
}

static char	*get_absolute_path(char *cmd)
{
	if (validate_command_path(cmd))
		return (ft_strdup(cmd));
	return (NULL);
}

static char	*get_relative_path(char *cmd, char **env)
{
	char	*path;

	path = find_command_path(cmd, env);
	return (path);
}

char	*get_command_full_path(char *cmd, char **env)
{
	if (!cmd || !cmd[0])
		return (NULL);
	if (cmd[0] == '/' || cmd[0] == '.')
		return (get_absolute_path(cmd));
	return (get_relative_path(cmd, env));
}
