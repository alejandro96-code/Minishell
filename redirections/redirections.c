/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/13 18:33:42 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Libera un array de strings
static void	free_string_array(char **array)
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
