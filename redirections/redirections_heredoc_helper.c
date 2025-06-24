/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_heredoc_helper.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 15:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/24 15:53:29 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal_received = 42;
}

// Escribir línea expandida al heredoc
void	write_expanded_line(int write_fd, char *line, char **env)
{
	char	*expanded_line;
	ssize_t	bytes_written;

	expanded_line = expand_variable(line, env, 0);
	bytes_written = write(write_fd, expanded_line, ft_strlen(expanded_line));
	if (bytes_written == -1)
		perror("write");
	bytes_written = write(write_fd, "\n", 1);
	if (bytes_written == -1)
		perror("write");
	free(expanded_line);
}

// Configurar signal handler para heredoc
void	setup_heredoc_signals(struct sigaction *old_sigint)
{
	struct sigaction	new_sigint;

	g_signal_received = 0;
	new_sigint.sa_handler = heredoc_sigint_handler;
	sigemptyset(&new_sigint.sa_mask);
	new_sigint.sa_flags = 0;
	sigaction(SIGINT, &new_sigint, old_sigint);
}

// Procesar una línea individual del heredoc
int	proc_sin_hdoc_line(int w_fd, char *line, char *limit, char **env)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	if (ft_strncmp(line, limit, ft_strlen(limit)) == 0
		&& ft_strlen(line) == ft_strlen(limit))
		return (1);
	write_expanded_line(w_fd, line, env);
	return (0);
}
