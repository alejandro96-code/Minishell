/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_char.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 13:17:20 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/07 20:14:43 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin_char(char *s1, char s2)
{
	char	*ptr;
	size_t	i;
	char	*original_s1;

	i = 0;
	original_s1 = s1;
	if (!s1)
		s1 = ft_strdup("");
	ptr = malloc(ft_strlen(s1) + 2);
	if (!ptr)
	{
		if (!original_s1)
			free(s1);
		return (NULL);
	}
	while (s1[i])
	{
		ptr[i] = s1[i];
		i++;
	}
	ptr[i] = s2;
	ptr[i + 1] = '\0';
	if (!original_s1)
		free(s1);
	return (ptr);
}

