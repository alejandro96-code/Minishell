/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logical_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 16:00:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	execute_single_command(t_command *cmd, char ***env)
{
	int		exit_status;
	pid_t	pid;
	int		status;
	int		i;
	char	*processed;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	
	// Expandir variables de entorno en todos los argumentos
	i = 0;
	while (i < cmd->argc && cmd->argv[i])
	{
		processed = process_quotes_and_variables(cmd->argv[i], *env);
		if (processed)
		{
			free(cmd->argv[i]);
			cmd->argv[i] = processed;
		}
		i++;
	}
	
	cmd->argv = expand_wildcards_in_args(cmd->argv, &cmd->argc);
	handle_redirections(&cmd->argv, *env);
	if (cmd->is_builtin)
		exit_status = execute_builtin(cmd->argv, env);
	else
	{
		pid = fork();
		if (pid == 0)
		{
			execute_external(cmd->argv, *env);
			exit(127); // Should not reach here if execute_external works correctly
		}
		else if (pid > 0)
		{
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else
				exit_status = 1;
		}
		else
		{
			perror("fork");
			exit_status = 1;
		}
	}
	g_exit_status = exit_status;
	return (exit_status);
}

static int	execute_and_operator(t_ast_node *node, char ***env)
{
	int	left_result;
	int	right_result;

	if (!node->left || !node->right)
		return (1);
	left_result = execute_ast(node->left, env);
	if (left_result != 0)
		return (left_result);
	right_result = execute_ast(node->right, env);
	return (right_result);
}

static int	execute_or_operator(t_ast_node *node, char ***env)
{
	int	left_result;
	int	right_result;

	if (!node->left || !node->right)
		return (1);
	left_result = execute_ast(node->left, env);
	if (left_result == 0)
		return (left_result);
	right_result = execute_ast(node->right, env);
	return (right_result);
}

int	execute_ast(t_ast_node *node, char ***env)
{
	if (!node)
		return (1);
	if (node->operator == OP_NONE && node->command)
		return (execute_single_command(node->command, env));
	else if (node->operator == OP_AND)
		return (execute_and_operator(node, env));
	else if (node->operator == OP_OR)
		return (execute_or_operator(node, env));
	return (1);
}
