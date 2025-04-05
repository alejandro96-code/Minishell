/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 19:22:43 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/11 10:50:10 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *n)
{
	size_t	i;

	i = 0;
	while (n[i] != '\0')
	{
		i++;
	}
	return (i);
}
/* 
#include <stdio.h>
int main(void)
{
	printf("%i\n",(int)ft_strlen("hola que tal ?"));
}
 */