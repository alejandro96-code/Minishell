/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 13:28:31 by alejanr2          #+#    #+#             */
/*   Updated: 2025/04/05 17:35:33 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft.h"

void	ft_putnbr(long nb, char *str, int *i)
{
	if (nb < 0)
	{
		nb *= -1;
		str[*i] = '-';
		(*i)++;
	}
	if (nb >= 10)
	{
		ft_putnbr(nb / 10, str, i);
		nb = nb % 10;
	}
	if (nb < 10)
	{
		str[*i] = (char)nb + '0';
		(*i)++;
	}
}

int	len(int nb)
{
	int	i;

	i = 0;
	if (nb <= 0)
		i++;
	while (nb)
	{
		nb = nb / 10;
		i++;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	char	*res;
	int		i;

	i = 0;
	res = (char *)malloc(sizeof(char) * (len(n) + 1));
	if (!res)
		return (NULL);
	ft_putnbr((long)n, res, &i);
	res[i] = '\0';
	return (res);
}
