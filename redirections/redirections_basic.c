/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_basic.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:00:00 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/19 01:56:10 by dgasco-g         ###   ########.fr       */
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

// Hacer backup de los file descriptors originales
int	backup_file_descriptors(void)
{
	int	original_stdout;

	original_stdout = dup(STDOUT_FILENO);
	if (original_stdout == -1)
		perror("dup stdout");
	return (original_stdout);
}

// Hacer backup de stdin
int	backup_stdin_fd(void)
{
	int	original_stdin;

	original_stdin = dup(STDIN_FILENO);
	if (original_stdin == -1)
		perror("dup stdin");
	return (original_stdin);
}

// Restaurar los file descriptors originales
void	restore_file_descriptors(int original_stdin, int original_stdout)
{
	if (original_stdin != -1)
	{
		if (dup2(original_stdin, STDIN_FILENO) == -1)
			perror("restore stdin");
		close(original_stdin);
	}
	if (original_stdout != -1)
	{
		if (dup2(original_stdout, STDOUT_FILENO) == -1)
			perror("restore stdout");
		close(original_stdout);
	}
}
