/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 23:44:26 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:25:51 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

char	*ft_strcat(char *dst, const char *orig)
{
	while (*dst)
		dst++;
	while (*orig)
	{
		*dst = *orig;
		dst++;
		orig++;
	}
	*dst = '\0';
	return (dst);
}
