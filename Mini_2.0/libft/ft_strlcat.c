/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 13:13:55 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:26:09 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	src_i;
	size_t	dest_i;
	size_t	i;

	src_i = ft_strlen((char *)src);
	if (!size)
		return (src_i + size);
	i = 0;
	while (dest[i] && i < size)
		i++;
	dest_i = i;
	while (src[i - dest_i] && i < size - 1)
	{
		dest[i] = src[i - dest_i];
		i++;
	}
	if (dest_i < size)
		dest[i] = '\0';
	return (dest_i + src_i);
}
