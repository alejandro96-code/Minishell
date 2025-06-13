/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_heredoc.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:00:00 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 00:00:00 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Escribir línea expandida al heredoc
static void	write_expanded_line(int write_fd, char *line, char **env)
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

// Leer entrada del heredoc hasta encontrar el delimitador
static void	read_heredoc_input(int write_fd, char *delimiter, char **env)
{
	char	*line;
	size_t	bufsize;
	size_t	len;

	line = NULL;
	bufsize = 0;
	printf("> ");
	while (getline(&line, &bufsize, stdin) != -1)
	{
		len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0 
			&& ft_strlen(line) == ft_strlen(delimiter))
			break ;
		write_expanded_line(write_fd, line, env);
		printf("> ");
	}
	free(line);
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