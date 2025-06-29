/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/29 04:19:21 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función para obtener el valor de una variable de entorno
char	*get_env_var(char *name, char **env)
{
	int		cont;
	size_t	name_len;

	cont = 0;
	name_len = ft_strlen(name);
	while (env[cont])
	{
		if (ft_strncmp(env[cont], name, name_len) == 0
			&& env[cont][name_len] == '=')
			return (&env[cont][name_len + 1]);
		cont++;
	}
	return (NULL);
}

// Cambia al directorio y libera memoria si es necesaria
static int	change_and_free(char *path, int should_free)
{
	if (chdir(path) != 0)
	{
		if (should_free)
			free(path);
		perror("cd");
		return (1);
	}
	if (should_free)
		free(path);
	return (0);
}

// Implementación del builtin cd (máximo 25 líneas)
int	builtin_cd(char **args, char **env)
{
	char	*path;
	int		should_free;

	should_free = 0;
	if (!args[1])
		path = handle_no_path_case(env, &should_free);
	else if (args[1][0] == '~')
		path = handle_tilde_path(args[1], env, &should_free);
	else if (args[1][0] != '/')
		path = handle_relative_path_case(args[1], &should_free);
	else
	{
		path = args[1];
		should_free = 0;
	}
	if (!path)
		return (1);
	return (change_and_free(path, should_free));
}
