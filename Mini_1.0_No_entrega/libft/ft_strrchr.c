/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/28 20:53:16 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/11/29 18:54:33 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

char	*ft_strrchr(const char *s, int c)
{
	int		x;
	char	*i;

	x = 0;
	i = NULL;
	c = c % 256;
	if (c == '\0')
	{
		while (s[x] != '\0')
			x++;
		return ((char *)(s + x));
	}
	while (s[x] != '\0')
	{
		if (s[x] == c)
		{
			i = (char *)(s + x);
		}
		x++;
	}
	return (i);
}
