/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 21:37:36 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/06 21:50:24 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putendl_fd(char *s, int fd)
{
	if (!s)
		return ;
	ft_putstr_fd(s, fd);
	write(fd, "\n", 1);
}

/* #include <fcntl.h>

int main (void)
{
	int fd;
	fd = open ("prueba.txt", O_RDWR);
 	char *s = "Hola dios";
	ft_putendl_fd(s, fd);
	return (0);
} */