/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 21:03:10 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/11 18:19:15 by dgasco-g         ###   ########.fr       */
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
	int fd;
	fd = open ("prueba.txt", O_RDWR);
	char *s = NULL;
	ft_putstr_fd(s, fd);
	return (0);
}
 */