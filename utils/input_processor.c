/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_processor.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:30:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	expand_command_args(t_command *cmd, char **env)
{
	int		i;
	char	*expanded;

	i = 0;
	while (i < cmd->argc)
	{
		expanded = expand_variable(cmd->argv[i], env);
		free(cmd->argv[i]);
		cmd->argv[i] = remove_quotes(expanded);
		i++;
	}
}

static void	process_single_command(t_command *cmd, char ***env)
{
	expand_command_args(cmd, *env);
	cmd->argv = expand_wildcards_in_args(cmd->argv, &cmd->argc);
	handle_redirections(&cmd->argv, *env);
	if (cmd->argv && cmd->argv[0])
	{
		if (cmd->is_builtin)
			execute_builtin(cmd->argv, env);
		else
			execute_external(cmd->argv, *env);
	}
}

void	process_input(char *input, char ***env)
{
	t_command	*cmd;

	g_signal_received = 0;
	if (ft_strchr(input, '|') != NULL)
	{
		run_command_pipeline(input, *env);
		free(input);
		return ;
	}
	cmd = parse_input(input);
	if (!cmd)
	{
		free(input);
		return ;
	}
	process_single_command(cmd, env);
	if (g_signal_received == SIGINT)
		g_signal_received = 0;
	else if (g_signal_received == SIGQUIT)
		g_signal_received = 0;
	free(input);
	free_command(cmd);
}

static void	handle_input(char *input, char ***env)
{
	if (*input)
	{
		add_history(input);
		process_input(input, env);
	}
	else
		free(input);
}

void	cleanup_and_exit(char **env)
{
	reset_signal_handlers();
	free_command_names();
	free_env(env);
}
