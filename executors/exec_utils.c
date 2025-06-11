/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 12:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/11 12:40:00 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_split(char **split)
{
	int	i;

	i = 0;
	if (!split)
		return ;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
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
