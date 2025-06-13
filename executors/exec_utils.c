/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 12:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/13 19:46:04 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Extraer paths del entorno
char	**extract_paths(char **envp)
{
	int		i;
	char	*path_var;

	i = 0;
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

// Buscar comando en los paths
char	*search_in_paths(char **paths, char *cmd)
{
	int		i;
	char	*temp;
	char	*cmd_path;

	if (!paths || !cmd)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(cmd_path, F_OK | X_OK) == 0)
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

char	*find_command_path(char *cmd, char **env)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	char	*temp;
	int		i;

	if (cmd[0] == '/' || cmd[0] == '.')
		return (ft_strdup(cmd));
	path_env = get_env_var("PATH", env);
	if (!path_env || !(paths = ft_split(path_env, ':')))
		return (NULL);
	i = -1;
	while (paths[++i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (full_path && access(full_path, X_OK) == 0)
		{
			ft_free_split(paths);
			return (full_path);
		}
		free(full_path);
	}
	ft_free_split(paths);
	return (NULL);
}

void	exec_child_process(char **args, char **env, char *cmd_path)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	execve(cmd_path, args, env);
	perror("execve");
	exit(127);
}

int	exec_parent_process(pid_t pid)
{
	int	status;
	int	sig;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
		{
			printf("\n");
			g_signal_received = SIGINT;
		}
		else if (sig == SIGQUIT)
		{
			printf("Quit (core dumped)\n");
			g_signal_received = SIGQUIT;
		}
		setup_signal_handlers();
		return (128 + sig);
	}
	else if (WIFEXITED(status))
	{
		setup_signal_handlers();
		return (WEXITSTATUS(status));
	}
	setup_signal_handlers();
	return (1);
}
