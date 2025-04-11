/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 16:53:35 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/07 20:38:17 by dgasco-g         ###   ########.fr       */
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
		return (ft_strjoin("unknown", "@Minishell~"));
	user = ft_strjoin(user, "@Minishell~");
	return (user);
}
