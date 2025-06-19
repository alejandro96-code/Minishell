/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/14 12:35:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/19 19:12:31 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_child_process(char **args, char **env, char *cmd_path)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	execve(cmd_path, args, env);
	perror("execve");
	exit(127);
}

static void	handle_signal_interrupt(int sig)
{
	ssize_t	result;

	if (sig == SIGINT)
	{
		result = write(1, "\n", 1);
		(void)result;
		g_signal_received = SIGINT;
	}
	else if (sig == SIGQUIT)
	{
		result = write(1, "Quit (core dumped)\n", 19);
		(void)result;
		g_signal_received = SIGQUIT;
	}
}

static int	handle_signaled_process(int status)
{
	int	sig;

	sig = WTERMSIG(status);
	handle_signal_interrupt(sig);
	setup_signal_handlers();
	return (128 + sig);
}

static int	handle_exited_process(int status)
{
	setup_signal_handlers();
	return (WEXITSTATUS(status));
}

int	exec_parent_process(pid_t pid)
{
	int	status;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		return (handle_signaled_process(status));
	else if (WIFEXITED(status))
		return (handle_exited_process(status));
	setup_signal_handlers();
	return (1);
}
