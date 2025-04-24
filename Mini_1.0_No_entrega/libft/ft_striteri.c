/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 20:43:40 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/11 17:47:28 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* 
void	ft_toupper_mod(unsigned int i, char *c)
{
	if (*c >= 'a' && *c <= 'z' && i % 2 == 0)
		*c = *c - 32;
}  */

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	unsigned int	i;

	i = 0;
	if (!s || !(*f))
		return ;
	while (s[i])
	{
		(*f)(i, &s[i]);
		i++;
	}
}

/* #include <stdio.h>
int main (void)
{
	char str[] = "hola";
	ft_striteri(str,NULL);
	printf("%s\n", str);
} */
