/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 20:57:34 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/13 02:01:13 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
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