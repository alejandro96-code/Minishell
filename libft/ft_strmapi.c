/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 13:36:02 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/06 20:42:40 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/* char	ft_toupper_mod(unsigned int i, char c)
{
	if (c >= 'a' && c <= 'z' && i % 2 == 0)
		return (c - 32);
	return (c);
} */

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*str;
	int		i;
	int		len;

	if (!s || !(*f))
		return (0);
	i = 0;
	len = ft_strlen(s);
	str = malloc(len + 1);
	if (!str)
		return (0);
	while (i < len)
	{
		str[i] = (*f)(i, s[i]);
		i++;
	}
	str[i] = '\0';
	return (str);
}

/* #include <stdio.h>
int main(void)
{
	char *result = ft_strmapi("hola", &ft_toupper_mod);
	if (result) {
		printf("%s\n", result);
		free(result);
	}
	return 0;
} */