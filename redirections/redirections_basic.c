/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_basic.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:00:00 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 00:00:00 by dgasco-g         ###   ########.fr       */
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