/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 00:11:21 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:24:43 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char	*result;

	result = (unsigned char *)malloc(nmemb * size);
	if (!result)
		return (0);
	ft_bzero((void *)result, nmemb * size);
	return ((void *)result);
}
