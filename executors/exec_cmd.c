/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 18:55:17 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*get_cmd_path_absolute(char *cmd)
{
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	printf("minishell: %s: No such file or directory\n", cmd);
	return (NULL);
}

static char	*get_cmd_path_relative(char *cmd, char **env)
{
	char	*cmd_path;

	cmd_path = find_command_path(cmd, env);
	if (!cmd_path)
		printf("%s: command not found\n", cmd);
	return (cmd_path);
}

static void	handle_fork_error(char *cmd_path)
{
	perror("fork");
	free(cmd_path);
}

static int	execute_fork_process(char **args, char **env, char *cmd_path)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		exec_child_process(args, env, cmd_path);
		return (0);
	}
	else if (pid > 0)
	{
		return (exec_parent_process(pid));
	}
	else
	{
		handle_fork_error(cmd_path);
		return (1);
	}
}

int	execute_external(char **args, char **env)
{
	char	*cmd_path;
	int		exit_code;

	if (!args || !args[0])
		return (1);
	if (ft_strchr(args[0], '/'))
		cmd_path = get_cmd_path_absolute(args[0]);
	else
		cmd_path = get_cmd_path_relative(args[0], env);
	if (!cmd_path)
		return (127);
	exit_code = execute_fork_process(args, env, cmd_path);
	free(cmd_path);
	return (exit_code);
}
