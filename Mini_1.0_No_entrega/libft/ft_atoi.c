/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/10 19:49:03 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

static int	ft_result(int x, int result, int sign)
{
	if (x > 1)
		return (0);
	return (result * sign);
}

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sign;
	int	result;
	int	x;

	x = 0;
	i = 0;
	sign = 1;
	result = 0;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	while (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = sign * -1;
		i++;
		x++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (ft_result(x, result, sign));
}
/* #include <stdlib.h>
int main(void)
{
	int	b = ft_atoi("&");
	int a = atoi("&");

	printf("%i\n", b);
	printf("%i\n", a);
	return (0);
}
 */