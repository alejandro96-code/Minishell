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
	int		i;

	user = NULL;
	i = 0;
	while (env[i] != NULL)
	{
		if (ft_strncmp(env[i], "USER=", 5) == 0)
		{
			user = env[i] + 5;
			break ;
		}
		i++;
	}
	if (!user)
		return (ft_strjoin("unknown", "@Minishell~"));
	user = ft_strjoin(user, "@Minishell~");
	return (user);
}
