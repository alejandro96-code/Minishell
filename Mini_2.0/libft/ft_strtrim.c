/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 21:43:25 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/07 17:03:00 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_in_set(char c, char *set)
{
	while (*set)
	{
		if (c == *set)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	size_t	start;
	size_t	end;
	char	*result;
	size_t	i;

	i = 0;
	start = 0;
	if (!s1 || !set)
		return (0);
	while (s1[start] && is_in_set(s1[start], (char *)set))
		start++;
	end = ft_strlen(s1);
	while (end > start && is_in_set(s1[end - 1], (char *)set))
		end--;
	result = malloc((end - start) + 1);
	if (!result)
		return (0);
	while (start < end)
	{
		result[i] = s1[start];
		i++;
		start++;
	}
	result[i] = '\0';
	return (result);
}
