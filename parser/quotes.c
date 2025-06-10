/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 17:31:40 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 18:10:26 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	parse_quotes(char **args)
{
	char	*arg;
	int		i;
	int		j;

	i = 0;

	while (args[i])
	{
		if (args[i][0] == '\"' && ft_strchr(args[i], '$'))
		{
			j = 0;
			while (args[i][j])
			{
				if (!args[i][j] == '$')
					arg = ft_strjoin_char(arg, args[i][j]);
				else
				{
					
				}
			}
		}
	}
}
