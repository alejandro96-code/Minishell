/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logical_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/13 19:06:43 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función principal simplificada para ejecutar un comando individual
static int	execute_single_command(t_command *cmd, char ***env)
{
	int		exit_status;
	int		saved_stdin;
	int		saved_stdout;
	char	*full_command;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	full_command = build_full_command(cmd);
	if (full_command && ft_strchr(full_command, '|') != NULL)
	{
		exit_status = run_command_pipeline(full_command, *env);
		free(full_command);
		return (exit_status);
	}
	free(full_command);
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	process_command_arguments(cmd, *env, 0);
	if (cmd->is_builtin)
		exit_status = execute_builtin_command(cmd, env, saved_stdin, saved_stdout);
	else
		exit_status = execute_external_command(cmd, env);
	close(saved_stdin);
	close(saved_stdout);
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
	if (node->operator== OP_NONE && node->command)
		return (execute_single_command(node->command, env));
	else if (node->operator== OP_AND)
		return (execute_and_operator(node, env));
	else if (node->operator== OP_OR)
		return (execute_or_operator(node, env));
	return (1);
}
