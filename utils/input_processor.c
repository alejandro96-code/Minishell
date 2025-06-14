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
		
		// Solo liberar si expanded es diferente de old_arg y cmd->argv[i]
		if (expanded != old_arg && expanded != cmd->argv[i])
			free(expanded);
		
		// Solo liberar old_arg si cmd->argv[i] es diferente
		if (cmd->argv[i] != old_arg)
			free(old_arg);
		
		i++;
	}
}

static void	process_single_command(t_command *cmd, char ***env, int *exit_status)
{
	int	original_stdout;
	
	expand_command_args(cmd, *env, *exit_status);
	
	// Hacer backup del file descriptor de stdout antes de las redirecciones
	original_stdout = backup_file_descriptors();
	
	handle_redirections(&cmd->argv, *env);
	if (cmd->argv && cmd->argv[0])
	{
		if (cmd->is_builtin)
			execute_builtin(cmd->argv, env, exit_status);
		else
			*exit_status = execute_external(cmd->argv, *env);
	}
	
	// Restaurar el file descriptor original de stdout
	restore_file_descriptors(-1, original_stdout);
}

void	process_input(char *input, char ***env, int *exit_status)
{
	t_command	*cmd;

	g_signal_received = 0;
	
	// Check for pipes
	if (ft_strchr(input, '|') != NULL)
	{
		*exit_status = run_command_pipeline(input, *env);
		free(input);
		return ;
	}
	
	// Single command
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
	reset_signal_handlers();
	free_env(env);
}
