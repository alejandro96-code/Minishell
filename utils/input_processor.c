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

static int	has_logical_operators(char *input)
{
	int	i;
	int	in_quotes;
	char	quote_char;

	i = 0;
	in_quotes = 0;
	quote_char = 0;
	while (input[i])
	{
		if (!in_quotes && (input[i] == '"' || input[i] == '\''))
		{
			in_quotes = 1;
			quote_char = input[i];
		}
		else if (in_quotes && input[i] == quote_char)
		{
			in_quotes = 0;
			quote_char = 0;
		}
		else if (!in_quotes)
		{
			if ((input[i] == '&' && input[i + 1] == '&') || 
				(input[i] == '|' && input[i + 1] == '|') ||
				input[i] == '(' || input[i] == ')')
				return (1);
		}
		i++;
	}
	return (0);
}

void	process_input(char *input, char ***env)
{
	t_command	*cmd;
	t_ast_node	*ast;
	int			exit_status;

	g_signal_received = 0;
	
	// Check for logical operators first
	if (has_logical_operators(input))
	{
		ast = parse_logical_expression(input);
		if (ast)
		{
			exit_status = execute_ast(ast, env);
			free_ast_node(ast);
		}
		free(input);
		return ;
	}
	
	// Check for pipes
	if (ft_strchr(input, '|') != NULL)
	{
		run_command_pipeline(input, *env);
		free(input);
		return ;
	}
	
	// Single command
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

void	cleanup_and_exit(char **env)
{
	reset_signal_handlers();
	free_env(env);
}
