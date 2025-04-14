/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_s1_free.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 01:39:32 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/14 13:16:56 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// char	*ft_strjoin_s1_free(char *s1, char *s2)
// {
// 	char	*ptr;
// 	size_t	i;
// 	int		j;

// 	j = 0;
// 	i = -1;
// 	if (!s1)
// 		s1 = ft_strdup("");
// 	ptr = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
// 	if (!ptr)
// 		return (free(ptr), NULL);
// 	while (s1[++i])
// 		ptr[i] = s1[i];
// 	while (s2[j])
// 	{
// 		ptr[i] = s2[j++];
// 		i++;
// 	}
// 	ptr[i] = '\0';
// 	return (free(s1), ptr);
// }

char *ft_strjoin_s1_free(char *s1, char *s2)
{
    char *ptr;
    size_t i;
    int j;

    j = 0;
    i = -1;
    if (!s1) {
        s1 = ft_strdup("");
        if (!s1)
            return NULL;
    }
    
    if (!s2)
        return s1; // Si s2 es NULL, simplemente devolvemos s1
        
    ptr = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
    if (!ptr) {
        free(s1);
        return NULL;
    }
    
    while (s1[++i])
        ptr[i] = s1[i];
    while (s2[j])
    {
        ptr[i] = s2[j++];
        i++;
    }
    ptr[i] = '\0';
    
    free(s1); // Liberamos s1 solo al final, cuando ya no lo necesitamos
    return ptr;
}
