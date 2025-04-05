/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 13:31:08 by alejanr2          #+#    #+#             */
/*   Updated: 2025/04/05 17:35:33 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char		*substring;
	size_t		j;

	if (s == NULL)
		return (NULL);
	if (start >= ft_strlen(s) || ft_strlen(s) == 0 || len == 0)
	{
		substring = malloc(sizeof(char) * 1);
		if (!substring)
			return (NULL);
		substring[0] = '\0';
		return (substring);
	}
	if (len >= (ft_strlen(s) - start))
		len = ft_strlen(s) - start;
	substring = malloc(sizeof(char) * ((len) + 1));
	if (!substring)
		return (NULL);
	j = 0;
	while (j < len && s[start] != '\0')
		substring[j++] = s[start++];
	substring[j] = '\0';
	return (substring);
}
