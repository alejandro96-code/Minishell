/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 21:44:02 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:25:39 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	char	*str;

	str = ft_itoa(n);
	if (!str)
		return ;
	write(fd, str, ft_strlen(str));
	free(str);
}
/* #include <fcntl.h>

int main (void)
{
	int fd;
	fd = open ("prueba.txt", O_RDWR);
	int s = 0;
	ft_putnbr_fd(s, fd);
	return (0);
} */