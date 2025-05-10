/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 18:22:50 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:25:28 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t				i;
	const unsigned char	*src2;
	unsigned char		*dest2;

	if (!dest && !src)
		return (NULL);
	i = 0;
	src2 = src;
	dest2 = dest;
	while (i < n)
	{
		dest2[i] = src2[i];
		i++;
	}
	return (dest);
}
