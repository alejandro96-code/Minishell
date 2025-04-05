/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 13:29:20 by alejanr2          #+#    #+#             */
/*   Updated: 2025/04/05 17:35:33 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	int				i;
	unsigned char	*a;

	i = 0;
	a = (unsigned char *)s;
	while (i < (int)n)
	{
		if (a[i] == (unsigned char)c)
			return ((void *)s + i);
		i++;
	}
	return (NULL);
}
