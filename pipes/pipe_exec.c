/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 15:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 15:00:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	process_command_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		args[i] = remove_quotes(args[i]);
		i++;
	}
}

char	**parse_command_arguments(char *command, char **env)
{
	char	**args;

	args = ft_split(command, ' ');
	process_command_args(args);
	handle_redirections(&args, env);
	return (args);
}

static void	execute_builtin_child(char **args, char ***env)
{
	exit(execute_builtin(args, env));
}

static void	execute_external_child(char **args, char **env)
{
	char	*cmd_path;

	cmd_path = get_path(args[0], env);
	if (!cmd_path)
	{
		write(STDERR_FILENO, args[0], ft_strlen(args[0]));
		write(STDERR_FILENO, ": command not found\n", 20);
		exit(EXIT_FAILURE);
	}
	execve(cmd_path, args, env);
	perror("execve");
	free(cmd_path);
	exit(EXIT_FAILURE);
}

void	execute_child_command(char *command, char ***env)
{
	char	**args;
	int		num_args;

	args = parse_command_arguments(command, *env);
	num_args = 0;
	while (args && args[num_args])
		num_args++;
	args = expand_wildcards_in_args(args, &num_args);
	if (args[0])
	{
		if (is_builtin(args[0]))
			execute_builtin_child(args, env);
		else
			execute_external_child(args, *env);
	}
	exit(EXIT_SUCCESS);
}
