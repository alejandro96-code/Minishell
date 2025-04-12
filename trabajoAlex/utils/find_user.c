/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_user.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 16:53:35 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/12 17:28:28 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char *find_user(char **env)
{
    char *user = NULL;
    int cont = 0;

    // Buscar variable USER en el entorno
    while (env[cont] != NULL)
    {
        if (ft_strncmp(env[cont], "USER=", 5) == 0)
        {
            user = env[cont] + 5;
            break;
        }
        cont++;
    }
    
    // Si no se encuentra USER, usar "unknown"
    if (!user)
        return (ft_strdup("unknown@Minishell~"));
    
    // Si se encuentra, concatenar con "@Minishell~"
    return (ft_strjoin(user, "@Minishell~"));
}
