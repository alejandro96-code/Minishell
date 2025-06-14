/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/14 13:01:47 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Verifica si existen redirecciones en los argumentos
int	check_redirections_exist(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		if ((ft_strncmp(args[i], "<", 1) == 0 && ft_strlen(args[i]) == 1)
			|| (ft_strncmp(args[i], "<<", 2) == 0 && ft_strlen(args[i]) == 2)
			|| (ft_strncmp(args[i], ">", 1) == 0 && ft_strlen(args[i]) == 1)
			|| (ft_strncmp(args[i], ">>", 2) == 0 && ft_strlen(args[i]) == 2))
			return (1);
		i++;
	}
	return (0);
}

// Cuenta el número de argumentos en el array
static int	count_string_args(char **args)
{
	int	count;

	count = 0;
	while (args[count])
		count++;
	return (count);
}

// Filtra los argumentos quitando las redirecciones y aplicándolas
void	handle_redirections(char ***args, char **env)
{
	char	**new_args;
	int		count;

	if (!args || !*args)
		return ;
	if (!check_redirections_exist(*args))
		return ;
	count = count_string_args(*args);
	new_args = create_filtered_args(*args, count, env);
	if (!new_args)
		return ;
	free_string_array(*args);
	*args = new_args;
}
