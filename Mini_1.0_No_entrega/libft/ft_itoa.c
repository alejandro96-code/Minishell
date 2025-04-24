/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 13:31:45 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/09 19:18:34 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static int	int_leng(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len++;
	while (n != 0)
	{
		n = n / 10;
		len++;
	}
	return (len);
}

static char	*convert(int n, int negative, int len)
{
	char	*str;

	str = ft_calloc(sizeof(char), (len + 1));
	if (!str)
		return (NULL);
	str[len] = '\0';
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n = n / 10;
	}
	if (negative)
		str[0] = '-';
	return (str);
}

char	*ft_itoa(int n)
{
	int	len;
	int	negative;

	if (n == 0)
		return (ft_strdup("0"));
	len = int_leng(n);
	negative = 0;
	if (n < 0)
	{
		if (n == -2147483648)
			return (ft_strdup("-2147483648"));
		else
		{
			n = -n;
			negative = 1;
		}
	}
	return (convert(n, negative, len));
}
//#include <stdio.h>
/* int main(int argc, char *argv[])
{
	char *resultado;
	if (argc != 2)
		return (0);
	int i = ft_atoi(argv[1]);
	char *resultado2;

	resultado = ft_itoa(i);
	printf("%s\n",resultado);
} */
/* 
int main(void)
{
	char *resultado;
	char *str = ft_strdup(NULL);
	int n = ft_atoi(str);

	resultado = ft_itoa(n);
	printf("%s\n",resultado);
	return (0);
} */