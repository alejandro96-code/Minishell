/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 19:49:16 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/11 18:14:12 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*x1;
	const unsigned char	*x2;
	size_t				i;

	if (!n)
		return (0);
	x1 = (const unsigned char *)s1;
	x2 = (const unsigned char *)s2;
	i = 0;
	while (i < n)
	{
		if (x1[i] != x2[i])
			return ((int)(x1[i] - x2[i]));
		i++;
	}
	return (0);
}

/* #include <stdio.h>

int main(void)
{
	int a = 0;
	printf("%i",ft_memcmp(NULL,"aaaaaaaaaaaaaaaaaaab",a));
} */