/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 18:56:54 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/11/29 22:38:42 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*s2;
	size_t				i;

	i = 0;
	s2 = (const unsigned char *)s;
	if (!n)
		return (0);
	while (i < n)
	{
		if (s2[i] == (unsigned char) c)
			return ((void *) &s2[i]);
		i++;
	}
	return (0);
}
