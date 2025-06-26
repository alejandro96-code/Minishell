/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_heredoc.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:00:00 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/26 13:36:04 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Procesar entrada del heredoc línea por línea
static void	process_heredoc_lines(int write_fd, char *delimiter, char **env)
{
	char	*line;
	ssize_t	result;

	result = write(1, "> ", 2);
	(void)result;
	line = get_next_line(STDIN_FILENO);
	while (line != NULL && g_signal_received != 42)
	{
		if (proc_sin_hdoc_line(write_fd, line, delimiter, env))
		{
			free(line);
			return ;
		}
		free(line);
		if (g_signal_received == 42)
			break ;
		result = write(1, "> ", 2);
		(void)result;
		line = get_next_line(STDIN_FILENO);
	}
	if (line)
		free(line);
}

// Leer entrada del heredoc hasta encontrar el delimitador usando get_next_line
static void	read_heredoc_input(int write_fd, char *delimiter, char **env)
{
	struct sigaction	old_sigint;

	setup_heredoc_signals(&old_sigint);
	process_heredoc_lines(write_fd, delimiter, env);
	g_signal_received = 0;
	sigaction(SIGINT, &old_sigint, NULL);
}

// Implementación del heredoc (<<): crea un pipe y redirige su lectura a stdin
int	heredoc(char *delimiter, char **env)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (1);
	}
	read_heredoc_input(pipefd[1], delimiter, env);
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}
