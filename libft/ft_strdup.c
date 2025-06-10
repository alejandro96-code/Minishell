/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:35:38 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:25:57 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*result;
	size_t	i;

	i = ft_strlen(s);
	result = malloc(i + 1);
	if (!result)
		return (NULL);
	ft_memcpy((void *)result, s, i + 1);
	return (result);
}

/* #include <string.h>
#include <stdio.h>

int	main(void)
{
	char	*str;
	char	*str2;

	str = ft_strdup(NULL);
	str2 = strdup(NULL);
	printf("%s\n", str);
	printf("%s\n", str2);
	free(str);
	free(str2);
	return (0);
}
 */