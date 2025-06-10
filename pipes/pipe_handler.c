/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 15:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 15:00:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	setup_child_pipes(int i, int cmd_count, int pipefd[2],
		int *prev_pipe)
{
	handle_child_process(i, cmd_count, pipefd, prev_pipe);
	setup_child_signals();
}

static void	setup_parent_pipes(int i, int cmd_count, int pipefd[2],
		int *prev_pipe)
{
	if (i > 0)
		close(*prev_pipe);
	if (i < cmd_count - 1)
	{
		close(pipefd[1]);
		*prev_pipe = pipefd[0];
	}
}

void	process_pipeline_step(int i, int cmd_count, char **commands,
		char **env)
{
	int			pipefd[2];
	static int	prev_pipe = STDIN_FILENO;
	pid_t		pid;

	if (i < cmd_count - 1 && pipe(pipefd) == -1)
	{
		perror("pipe");
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		setup_child_pipes(i, cmd_count, pipefd, &prev_pipe);
		execute_child_command(commands[i], &env);
	}
	else if (pid > 0)
		setup_parent_pipes(i, cmd_count, pipefd, &prev_pipe);
	else
		perror("fork");
}

static void	wait_for_children(int cmd_count)
{
	int	i;
	int	status;

	i = 0;
	while (i < cmd_count)
	{
		wait(&status);
		i++;
	}
}

int	run_command_pipeline(char *input, char **env)
{
	char	**commands;
	int		cmd_count;
	int		i;

	cmd_count = split_and_validate_commands(input, &commands);
	if (cmd_count <= 0 || !commands)
		return (1);
	i = 0;
	while (i < cmd_count)
	{
		process_pipeline_step(i, cmd_count, commands, env);
		i++;
	}
	wait_for_children(cmd_count);
	ft_free_split(commands);
	return (0);
}
