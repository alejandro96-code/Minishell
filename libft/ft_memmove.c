/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 20:24:01 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:25:30 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*dest2;
	const unsigned char	*src2;

	dest2 = dest;
	src2 = src;
	if (dest == src)
		return (dest);
	if (dest2 <= src2)
	{
		while (n--)
		{
			*dest2++ = *src2++;
		}
	}
	else
	{
		dest2 += n - 1;
		src2 += n - 1;
		while (n--)
		{
			*dest2-- = *src2--;
		}
	}
	return (dest);
}
