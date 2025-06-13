/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expression_parser.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/13 21:20:00 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Construye un comando completo a partir de tokens
static char	*build_command_string(char **tokens, int *index)
{
	char	*full_command;
	char	*temp;

	full_command = ft_strdup("");
	while (tokens[*index] && !is_logical_operator(tokens[*index])
		&& !is_parenthesis_token(tokens[*index]))
	{
		temp = full_command;
		if (ft_strlen(full_command) > 0)
			full_command = ft_strjoin(temp, " ");
		else
			full_command = ft_strdup("");
		free(temp);
		temp = full_command;
		full_command = ft_strjoin(temp, tokens[*index]);
		free(temp);
		(*index)++;
	}
	return (full_command);
}

// Parsea una expresión primaria (comando o expresión entre paréntesis)
t_ast_node	*parse_primary_command(char **tokens, int *index)
{
	t_ast_node	*node;
	t_command	*cmd;
	char		*full_command;

	if (!tokens[*index])
		return (NULL);
	if (ft_strncmp(tokens[*index], "(", 2) == 0)
	{
		(*index)++;
		node = parse_or_operation(tokens, index);
		if (node)
			node->in_parentheses = 1;
		if (tokens[*index] && ft_strncmp(tokens[*index], ")", 2) == 0)
			(*index)++;
		return (node);
	}
	full_command = build_command_string(tokens, index);
	if (ft_strlen(full_command) == 0)
	{
		free(full_command);
		return (NULL);
	}
	cmd = parse_command_input(full_command, NULL);
	free(full_command);
	if (!cmd)
		return (NULL);
	node = create_ast_node(OP_NONE, cmd);
	return (node);
}

// Parsea expresiones con operador AND
t_ast_node	*parse_and_operation(char **tokens, int *index)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*and_node;

	left = parse_primary_command(tokens, index);
	if (!left)
		return (NULL);
	while (tokens[*index] && ft_strncmp(tokens[*index], "&&", 3) == 0)
	{
		(*index)++;
		right = parse_primary_command(tokens, index);
		if (!right)
		{
			free_ast_node(left);
			return (NULL);
		}
		and_node = create_ast_node(OP_AND, NULL);
		if (!and_node)
		{
			free_ast_node(left);
			free_ast_node(right);
			return (NULL);
		}
		and_node->left = left;
		and_node->right = right;
		left = and_node;
	}
	return (left);
}

// Parsea expresiones con operador OR  
t_ast_node	*parse_or_operation(char **tokens, int *index)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*or_node;

	left = parse_and_operation(tokens, index);
	if (!left)
		return (NULL);
	while (tokens[*index] && ft_strncmp(tokens[*index], "||", 3) == 0)
	{
		(*index)++;
		right = parse_and_operation(tokens, index);
		if (!right)
		{
			free_ast_node(left);
			return (NULL);
		}
		or_node = create_ast_node(OP_OR, NULL);
		if (!or_node)
		{
			free_ast_node(left);
			free_ast_node(right);
			return (NULL);
		}
		or_node->left = left;
		or_node->right = right;
		left = or_node;
	}
	return (left);
}

// Función principal de parsing de expresiones lógicas
t_ast_node	*parse_logical_expression(const char *input)
{
	char		**tokens;
	t_ast_node	*ast;
	int			index;

	if (!input || !*input)
		return (NULL);
	tokens = tokenize_expression(input);
	if (!tokens)
		return (NULL);
	index = 0;
	ast = parse_or_operation(tokens, &index);
	free_tokens(tokens);
	return (ast);
}