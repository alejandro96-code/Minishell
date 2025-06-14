/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expression_executor.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/14 09:34:31 by alejandro        ###   ########.fr       */
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
	
	// Verificar si es un pipeline
	full_command = build_full_command(cmd);
	if (full_command && ft_strchr(full_command, '|') != NULL)
	{
		exit_status = run_command_pipeline(full_command, *env);
		free(full_command);
		return (exit_status);
	}
	free(full_command);
	
	// Guardar descriptores de archivo
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	
	// Procesar argumentos
	process_command_args(cmd, *env, 0);
	
	// Ejecutar comando según tipo
	if (cmd->is_builtin)
		exit_status = run_builtin_command(cmd, env, saved_stdin, saved_stdout);
	else
		exit_status = run_external_command(cmd, env);
	
	// Restaurar descriptores
	close(saved_stdin);
	close(saved_stdout);
	return (exit_status);
}

int	execute_expression_tree(t_ast_node *node, char ***env)
{
	if (!node)
		return (1);
	if (node->operator == OP_NONE && node->command)
		return (execute_single_command(node->command, env));
	return (1);
}