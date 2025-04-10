/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 20:14:19 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/20 14:40:10 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static int	words_num(char *s, char c)
{
	int	i;
	int	x;
	int	in_words;

	in_words = 0;
	x = 0;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			in_words = 0;
		else
		{
			if (in_words == 0)
			{
				x++;
				in_words = 1;
			}
		}
		i++;
	}
	return (x);
}

static int	word_len(char *s, char c)
{
	int	i;
	int	x;

	x = 0;
	i = 0;
	while (s[i] == c)
		i++;
	while (s[i] != '\0' && s[i] != c)
	{
		x++;
		i++;
	}
	return (x);
}

static char	*word_copy(char *s, char c)
{
	int		i;
	char	*str;
	int		word_l;
	int		x;

	x = 0;
	word_l = word_len(s, c);
	str = malloc(word_l + 1);
	if (!str)
		return (NULL);
	i = 0;
	while (s[i] == c)
		i++;
	while (i < word_l)
	{
		str[x] = s[i];
		x++;
		i++;
	}
	str[i] = '\0';
	return (str);
}

static int	aux_1(char **result, int x)
{
	if (!result[x])
	{
		while (x >= 0)
			free(result[x--]);
		free(result);
		return (0);
	}
	return (1);
}

char	**ft_split(char const *s, char c)
{
	char	**result;
	int		j;
	int		x;
	int		i;

	if (!s)
		return (NULL);
	i = words_num((char *)s, c);
	x = 0;
	j = 0;
	result = malloc (sizeof(char *) * (i + 1));
	if (!result)
		return (NULL);
	while (x < i)
	{
		while (s[j] == c)
			j++;
		result[x] = word_copy((char *)&s[j], c);
		if (!aux_1(result, x))
			return (NULL);
		j += word_len((char *)&s[j], c);
		x++;
	}
	result[x] = NULL;
	return (result);
}

/* int	main(void)
{
	char	**test;
	int		i;

	i = 0;
	test = ft_split("awana-bum-bam-ban-puchuradio", '-');
	if (!test)
		return (0);
	while (test[i])
		printf("%s\n", test[i++]);
	i = 0;
	while(test[i])
		free(test[i++]);
	free(test);
	return (0);
}
 */