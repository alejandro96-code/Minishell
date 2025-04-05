/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 13:29:30 by alejanr2          #+#    #+#             */
/*   Updated: 2025/04/05 17:35:33 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	int		i;
	char	*str;

	i = 0;
	if (!dst && !src)
		return (0);
	str = (char *)dst;
	while (i < (int)n)
	{
		str[i] = ((char *)src)[i];
		i++;
	}
	return (dst);
}
