/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:35:14 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Redirigir la entrada desde un archivo
int	redirect_input(char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror(filename);
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

// Redirigir la salida a un archivo
int	redirect_output(char *filename, int append)
{
	int	flags;
	int	fd;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(filename, flags, 0644);
	if (fd < 0)
	{
		perror(filename);
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

// Implementación del heredoc (<<) con expansión de variables
static void	write_expanded_line(int write_fd, char *line, char **env)
{
	char	*expanded_line;
	ssize_t	bytes_written;

	expanded_line = expand_variable(line, env);
	bytes_written = write(write_fd, expanded_line, ft_strlen(expanded_line));
	if (bytes_written == -1)
		perror("write");
	bytes_written = write(write_fd, "\n", 1);
	if (bytes_written == -1)
		perror("write");
	free(expanded_line);
}

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

// Función para procesar las redirecciones en los argumentos
// Procesa una redirección individual según el tipo y avanza el índice
static void	handle_redirection_type(char **args, int *i, char *cleaned_filename,
		char **env)
{
	char	*cleaned_delimiter;

	if (ft_strncmp(args[*i], "<", 1) == 0 && ft_strlen(args[*i]) == 1)
		redirect_input(cleaned_filename);
	else if (ft_strncmp(args[*i], "<<", 2) == 0 && ft_strlen(args[*i]) == 2)
	{
		cleaned_delimiter = remove_quotes(args[*i + 1]);
		heredoc(cleaned_delimiter, env);
		if (cleaned_delimiter != args[*i + 1])
			free(cleaned_delimiter);
	}
	else if (ft_strncmp(args[*i], ">", 1) == 0 && ft_strlen(args[*i]) == 1)
		redirect_output(cleaned_filename, 0);
	else if (ft_strncmp(args[*i], ">>", 2) == 0 && ft_strlen(args[*i]) == 2)
		redirect_output(cleaned_filename, 1);
	(*i)++;
}

static void	process_redirection(char **args, int *i, char **env)
{
	char	*expanded_filename;
	char	*cleaned_filename;

	if (!args[*i + 1])
		return ;
	expanded_filename = expand_variable(args[*i + 1], env);
	cleaned_filename = remove_quotes(expanded_filename);
	handle_redirection_type(args, i, cleaned_filename, env);
	if (expanded_filename != args[*i + 1])
		free(expanded_filename);
	if (cleaned_filename != expanded_filename)
		free(cleaned_filename);
}

// Filtra los argumentos quitando las redirecciones y aplicándolas
void	handle_redirections(char ***args, char **env)
{
	char	**new_args;
	int		i;
	int		j;
	int		count;
	int		has_redirections;

	// Primero verificar si hay redirecciones
	has_redirections = 0;
	i = 0;
	while ((*args)[i])
	{
		if ((ft_strncmp((*args)[i], "<", 1) == 0 && ft_strlen((*args)[i]) == 1) 
			|| (ft_strncmp((*args)[i], "<<", 2) == 0 && ft_strlen((*args)[i]) == 2)
			|| (ft_strncmp((*args)[i], ">", 1) == 0 && ft_strlen((*args)[i]) == 1) 
			|| (ft_strncmp((*args)[i], ">>", 2) == 0 && ft_strlen((*args)[i]) == 2))
		{
			has_redirections = 1;
			break ;
		}
		i++;
	}

	// Si no hay redirecciones, no hacer nada
	if (!has_redirections)
		return ;

	count = 0;
	while ((*args)[count])
		count++;
	new_args = malloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return ;
	i = 0;
	j = 0;
	while ((*args)[i])
	{
		if ((ft_strncmp((*args)[i], "<", 1) == 0 && ft_strlen((*args)[i]) == 1) 
			|| (ft_strncmp((*args)[i], "<<", 2) == 0 && ft_strlen((*args)[i]) == 2)
			|| (ft_strncmp((*args)[i], ">", 1) == 0 && ft_strlen((*args)[i]) == 1) 
			|| (ft_strncmp((*args)[i], ">>", 2) == 0 && ft_strlen((*args)[i]) == 2))
		{
			process_redirection(*args, &i, env);
			i++;
			continue ;
		}
		new_args[j++] = (*args)[i++];
	}
	new_args[j] = NULL;
	free(*args);
	*args = new_args;
}
