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

static void	process_pipe_command_args(char **args, char **env)
{
	int		i;
	char	*expanded;
	char	*old_arg;

	i = 0;
	while (args[i])
	{
		old_arg = args[i];
		expanded = process_quotes_and_variables(args[i], env, 0);
		if (expanded)
		{
			args[i] = expanded;
			if (old_arg != expanded)
				free(old_arg);
		}
		else
		{
			args[i] = remove_quotes(args[i]);
		}
		i++;
	}
}

char	**parse_command_arguments(char *command, char **env)
{
	char	**args;

	args = ft_split(command, ' ');
	process_pipe_command_args(args, env);
	handle_redirections(&args, env);
	return (args);
}

static void	execute_builtin_child(char **args, char ***env)
{
	int	exit_status;

	exit_status = 0;
	execute_builtin(args, env, &exit_status);
	exit(exit_status);
}

static void	execute_external_child(char **args, char **env)
{
	char	*cmd_path;

	cmd_path = find_command_path(args[0], env);
	if (!cmd_path)
	{
		write_command_not_found_error(args[0]);
		exit(127);  // Command not found
	}
	execve(cmd_path, args, env);
	perror("execve");
	free(cmd_path);
	exit(127);  // execve failed
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
