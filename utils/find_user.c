/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_user.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/14 13:03:50 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*find_user(char **env)
{
	char	*user;
	int		cont;

	user = NULL;
	cont = 0;
	while (env[cont] != NULL)
	{
		if (ft_strncmp(env[cont], "USER=", 5) == 0)
		{
			user = env[cont] + 5;
			break ;
		}
		cont++;
	}
	if (!user)
		return (ft_strdup("unknown"));
	return (ft_strjoin(user, "@Minishell~"));
}
