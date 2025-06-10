/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 15:14:57 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_commands_and_split(char *input, char ***commands)
{
	return (split_and_validate_commands(input, commands));
}

char	**parse_args_and_handle(char *command, char **env)
{
	return (parse_command_arguments(command, env));
}

void	child_exec_or_builtin(char *command, char ***env)
{
	execute_child_command(command, env);
}

void	handle_pipeline_iteration(int i, int cmd_count, char **commands,
		char **env)
{
	process_pipeline_step(i, cmd_count, commands, env);
}

int	execute_pipeline(char *input, char **env)
{
	return (run_command_pipeline(input, env));
}
