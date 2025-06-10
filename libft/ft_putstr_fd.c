/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 21:03:10 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:25:42 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	if (!s)
		return ;
	i = ft_strlen(s);
	write(fd, s, i);
}

/* #include <fcntl.h>
int main (void)
{
	int		fd;
	char	*s;

	fd = open ("prueba.txt", O_RDWR);
	s = NULL;
	ft_putstr_fd(s, fd);
	return (0);
}
 */