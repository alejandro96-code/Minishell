/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 18:22:06 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:26:19 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned int	x;

	x = 0;
	if (!n)
		return (0);
	x = 0;
	while ((s1[x] != '\0' && s2[x] != '\0') && x < n)
	{
		if (s1[x] != s2[x])
			return (s1[x] - s2[x]);
		x++;
	}
	if (x == n)
		return (0);
	return (s1[x] - s2[x]);
}

/* #include <stdio.h>

int	main(void)
{
	printf("%i\n", ft_strncmp("", "", 2));
}
 */