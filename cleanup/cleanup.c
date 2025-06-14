/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 20:00:00 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/14 09:26:08 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


// Libera un array de strings (como char **)
void	ft_free_split(char **split)
{
	int	i;
	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

// Libera un array de strings genérico
void	free_string_array(char **array)
{
	int	i;
	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

// Libera las variables de entorno
void	free_env(char **env)
{
	ft_free_split(env);
}

// Libera un array de argumentos
char	**free_args(char **args)
{
	if (args)
		free_string_array(args);
	return (NULL);
}

// Libera una estructura t_command completa
void	free_command(t_command *cmd)
{
	int	i;
	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
	}
	free(cmd);
}