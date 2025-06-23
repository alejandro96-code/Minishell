/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/24 18:31:47 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/23 17:34:51 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	gnl_strlen(char const *s)
{
	size_t	len;

	len = 0;
	while (s[len] != '\0')
		len++;
	return (len);
}

char	*gnl_strjoin(char *s1, char *s2)
{
	char	*s3;
	size_t	cont1;
	size_t	cont2;

	s3 = malloc(gnl_strlen(s1) + gnl_strlen(s2) + 1);
	cont1 = 0;
	while (s1[cont1] != '\0')
	{
		s3[cont1] = s1[cont1];
		cont1++;
	}
	cont2 = 0;
	while (s2[cont2] != '\0')
	{
		s3[cont1] = s2[cont2];
		cont1++;
		cont2++;
	}
	s3[cont1] = '\0';
	free(s1);
	return (s3);
}
