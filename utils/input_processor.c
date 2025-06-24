/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_processor.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/24 14:13:41 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Detecta si hay pipes fuera de comillas
static int	has_unquoted_pipes(const char *input)
{
	int	i;
	int	in_single_quotes;
	int	in_double_quotes;

	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (input[i])
	{
		if (input[i] == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		else if (input[i] == '"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (input[i] == '|' && !in_single_quotes && !in_double_quotes)
			return (1);
		i++;
	}
	return (0);
}

static void	expand_command_args(t_command *cmd, char **env, int exit_status)
{
	int		i;
	char	*expanded;
	char	*old_arg;

	i = 0;
	while (i < cmd->argc)
	{
		old_arg = cmd->argv[i];
		expanded = expand_variable(cmd->argv[i], env, exit_status);
		cmd->argv[i] = remove_quotes(expanded);
		if (expanded != old_arg && expanded != cmd->argv[i])
			free(expanded);
		if (cmd->argv[i] != old_arg)
			free(old_arg);
		i++;
	}
}

static void	process_single_command(t_command *cmd, char ***env,
		int *exit_status)
{
	int	original_stdin;
	int	original_stdout;

	expand_command_args(cmd, *env, *exit_status);
	original_stdin = backup_stdin_fd();
	original_stdout = backup_file_descriptors();
	handle_redirections(&cmd->argv, *env);
	if (cmd->argv && cmd->argv[0])
	{
		if (cmd->is_builtin)
			execute_builtin(cmd->argv, env, exit_status);
		else
			*exit_status = execute_external(cmd->argv, *env);
	}
	restore_file_descriptors(original_stdin, original_stdout);
}

void	process_input(char *input, char ***env, int *exit_status)
{
	t_command	*cmd;

	g_signal_received = 0;
	if (has_unquoted_pipes(input))
	{
		*exit_status = run_command_pipeline(input, *env);
		free(input);
		return ;
	}
	cmd = parse_command_input(input, exit_status);
	if (!cmd)
	{
		free(input);
		return ;
	}
	process_single_command(cmd, env, exit_status);
	if (g_signal_received == SIGINT)
		g_signal_received = 0;
	else if (g_signal_received == SIGQUIT)
		g_signal_received = 0;
	free(input);
	free_command(cmd);
}

void	cleanup_and_exit(char **env)
{
	reset_signal_to_default();
	free_env(env);
}
