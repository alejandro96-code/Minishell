/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/24 18:31:47 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/23 17:34:50 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	ft_strchr(char *s1, int nl)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0')
	{
		if (s1[i] == nl)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_line(char *s)
{
	char	*line;
	size_t	i;

	i = 0;
	if (s[i] == '\0')
		return (NULL);
	while (s[i] != '\0' && s[i] != '\n')
	{
		i++;
	}
	line = malloc(i + 2);
	i = 0;
	while (s[i] != '\0' && s[i] != '\n')
	{
		line[i] = s[i];
		i++;
	}
	if (s[i] == '\n')
	{
		line[i] = '\n';
		line[i + 1] = '\0';
		return (line);
	}
	return (line[i] = '\0', line);
}

char	*ft_remainder(char *s)
{
	size_t	i;
	size_t	j;
	char	*remainder;
	char	*ptr;

	i = 0;
	while (s[i] != '\n' && s[i] != '\0')
		i++;
	if (s[i] == '\n')
	{
		ptr = &s[i + 1];
		remainder = malloc(gnl_strlen((char *)ptr) + 1);
		j = 0;
		while (s[i + 1] != '\0')
		{
			remainder[j] = s[i + 1];
			i++;
			j++;
		}
		remainder[j] = '\0';
		free(s);
		return (remainder);
	}
	return (free(s), remainder = malloc(1), remainder[0] = '\0', remainder);
}

char	*read_loop(char *buffer, int bytes_read, char *stash, int fd)
{
	int	i;

	while (ft_strchr(buffer, '\n') == 0 && bytes_read == BUFFER_SIZE)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		buffer[bytes_read] = '\0';
		stash = gnl_strjoin(stash, buffer);
		i = 0;
		while (buffer[i] != '\0')
		{
			buffer[i] = '\0';
			i++;
		}
	}
	return (stash);
}

char	*get_next_line(int fd)
{
	char		*buffer;
	int			bytes_read;
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	buffer = malloc(BUFFER_SIZE + 1);
	buffer[0] = '\0';
	if (stash == NULL)
	{
		stash = malloc(1);
		stash[0] = '\0';
	}
	bytes_read = BUFFER_SIZE;
	stash = read_loop(buffer, bytes_read, stash, fd);
	if (bytes_read == -1)
		return (NULL);
	line = ft_line(stash);
	stash = ft_remainder(stash);
	free(buffer);
	if (stash[0] == '\0')
		return (free(stash), stash = NULL, line);
	return (line);
}
