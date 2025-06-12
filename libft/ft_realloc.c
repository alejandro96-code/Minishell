/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 21:42:26 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/12 21:52:48 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_realloc(void *s, size_t size)
{
	void	*new_ptr;

	if (!s)
		return (ft_calloc(1, size));
	if (size == 0)
	{
		free(s);
		return (NULL);
	}
	new_ptr = ft_calloc(1, size);
	if (!new_ptr)
		return (NULL);
	ft_memcpy(new_ptr, s, size);
	free(s);
	return (new_ptr);
}
