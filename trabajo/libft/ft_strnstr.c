/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 22:41:57 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/15 00:56:20 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	j;
	size_t	i;

	i = 0;
	if (!big)
		return (NULL);
	if (!little || little[0] == '\0')
		return ((char *)big);
	if (!len)
		return (NULL);
	while (big[i] != '\0' && i < len)
	{
		j = 0;
		while ((j + i) < len && big[i + j] == little[j])
		{
			j++;
			if (little[j] == '\0')
				return ((char *)&big[i]);
		}
		i++;
	}
	return (NULL);
}
/*
#include <stdio.h>
#include <string.h>

int main(void)
{
	//char *prueba = NULL;
	printf("%s\n", ft_strnstr("hola", "", 17));
	//printf("%i\n", strnstr("hola", "hola que tal estas ", 17));
}
 */