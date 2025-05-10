/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 18:26:24 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:24:55 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isdigit(int n)
{
	if (n < '0' || n > '9')
	{
		return (0);
	}
	return (1);
}
/*
int main (int argc, char *argv[])
{
	int w = atoi(argv[1]);
	printf("%i", ft_isdigit(w));
}*/
