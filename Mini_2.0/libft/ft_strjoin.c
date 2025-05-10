/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 20:05:57 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:26:07 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*res;
	size_t	s1_l;
	size_t	s2_l;

	if (!s1 || !s2)
		return (0);
	s1_l = ft_strlen(s1);
	s2_l = ft_strlen(s2);
	res = malloc((sizeof(char) * (s1_l + s2_l + 1)));
	if (!res)
		return (0);
	ft_memcpy(res, s1, s1_l);
	ft_memcpy(res + s1_l, s2, s2_l);
	res[s1_l + s2_l] = '\0';
	return (res);
}
