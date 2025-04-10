/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:35:38 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/10 13:55:07 by dgasco-g         ###   ########.fr       */
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
int main(void)
{
	char *str = ft_strdup(NULL);
	char *str2 = strdup(NULL);
	
	printf("%s\n", str);
	printf("%s\n", str2);
	free(str);
	free(str2);
	return (0);
}
 */