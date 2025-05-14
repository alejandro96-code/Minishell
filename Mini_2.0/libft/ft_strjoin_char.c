/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_srtjoin_char.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 13:17:20 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/14 18:49:48 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin_char(char *s1, char s2)
{
	char	*ptr;
	size_t	i;
	int		j;

	j = 0;
	i = -1;
	if (!s1)
		s1 = ft_strdup("");
	ptr = malloc(ft_strlen(s1) + 2);
	if (!ptr)
		return (free(ptr), NULL);
	while (s1[++i])
		ptr[i] = s1[i];
	ptr[i] = s2;
	i++;
	ptr[i] = '\0';
	return (free(s1), ptr);
}

