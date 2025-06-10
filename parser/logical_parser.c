/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logical_parser.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 16:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 16:00:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast_node	*create_ast_node(t_operator_type op, t_command *cmd)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->operator = op;
	node->command = cmd;
	node->left = NULL;
	node->right = NULL;
	node->in_parentheses = 0;
	return (node);
}

void	free_ast_node(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->left)
		free_ast_node(node->left);
	if (node->right)
		free_ast_node(node->right);
	if (node->command)
		free_command(node->command);
	free(node);
}

static int	is_operator(const char *token)
{
	if (!token)
		return (0);
	return (ft_strncmp(token, "&&", 3) == 0 || ft_strncmp(token, "||", 3) == 0);
}

static int	is_parenthesis(const char *token)
{
	if (!token)
		return (0);
	return (ft_strncmp(token, "(", 2) == 0 || ft_strncmp(token, ")", 2) == 0);
}

static void	add_token_to_list(char ***tokens, int *count, char *token)
{
	char	**new_tokens;
	int		i;

	new_tokens = malloc(sizeof(char *) * (*count + 2));
	if (!new_tokens)
		return ;
	i = 0;
	while (i < *count)
	{
		new_tokens[i] = (*tokens)[i];
		i++;
	}
	new_tokens[*count] = ft_strdup(token);
	new_tokens[*count + 1] = NULL;
	free(*tokens);
	*tokens = new_tokens;
	(*count)++;
}

char	**tokenize_input(const char *input)
{
	char	**tokens;
	char	*token;
	int		count;
	int		i;
	int		start;

	tokens = malloc(sizeof(char *));
	tokens[0] = NULL;
	count = 0;
	i = 0;
	while (input[i])
	{
		while (input[i] == ' ' || input[i] == '\t')
			i++;
		if (!input[i])
			break ;
		start = i;
		if (input[i] == '(' || input[i] == ')')
		{
			token = ft_substr(input, i, 1);
			add_token_to_list(&tokens, &count, token);
			free(token);
			i++;
		}
		else if (input[i] == '&' && input[i + 1] == '&')
		{
			token = ft_strdup("&&");
			add_token_to_list(&tokens, &count, token);
			free(token);
			i += 2;
		}
		else if (input[i] == '|' && input[i + 1] == '|')
		{
			token = ft_strdup("||");
			add_token_to_list(&tokens, &count, token);
			free(token);
			i += 2;
		}
		else
		{
			while (input[i] && input[i] != ' ' && input[i] != '\t' 
				&& input[i] != '(' && input[i] != ')' 
				&& !(input[i] == '&' && input[i + 1] == '&')
				&& !(input[i] == '|' && input[i + 1] == '|'))
			{
				if (input[i] == '"' || input[i] == '\'')
				{
					char quote = input[i++];
					while (input[i] && input[i] != quote)
						i++;
					if (input[i] == quote)
						i++;
				}
				else
					i++;
			}
			token = ft_substr(input, start, i - start);
			add_token_to_list(&tokens, &count, token);
			free(token);
		}
	}
	return (tokens);
}

void	free_tokens(char **tokens)
{
	int	i;

	if (!tokens)
		return ;
	i = 0;
	while (tokens[i])
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

t_ast_node	*parse_primary_expression(char **tokens, int *index)
{
	t_ast_node	*node;
	t_command	*cmd;
	char		*full_command;
	int			cmd_start;

	if (!tokens[*index])
		return (NULL);
	if (ft_strncmp(tokens[*index], "(", 2) == 0)
	{
		(*index)++;
		node = parse_or_expression(tokens, index);
		if (node)
			node->in_parentheses = 1;
		if (tokens[*index] && ft_strncmp(tokens[*index], ")", 2) == 0)
			(*index)++;
		return (node);
	}
	
	// Collect all tokens that form a single command until we hit an operator or parenthesis
	cmd_start = *index;
	full_command = ft_strdup("");
	while (tokens[*index] && !is_operator(tokens[*index]) && !is_parenthesis(tokens[*index]))
	{
		char *temp = full_command;
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
	
	if (ft_strlen(full_command) == 0)
	{
		free(full_command);
		return (NULL);
	}
	
	cmd = parse_input(full_command, NULL);
	free(full_command);
	if (!cmd)
		return (NULL);
	node = create_ast_node(OP_NONE, cmd);
	return (node);
}

t_ast_node	*parse_and_expression(char **tokens, int *index)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*and_node;

	left = parse_primary_expression(tokens, index);
	if (!left)
		return (NULL);
	while (tokens[*index] && ft_strncmp(tokens[*index], "&&", 3) == 0)
	{
		(*index)++;
		right = parse_primary_expression(tokens, index);
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

t_ast_node	*parse_or_expression(char **tokens, int *index)
{
	t_ast_node	*left;
	t_ast_node	*right;
	t_ast_node	*or_node;

	left = parse_and_expression(tokens, index);
	if (!left)
		return (NULL);
	while (tokens[*index] && ft_strncmp(tokens[*index], "||", 3) == 0)
	{
		(*index)++;
		right = parse_and_expression(tokens, index);
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

t_ast_node	*parse_logical_expression(const char *input)
{
	char		**tokens;
	t_ast_node	*ast;
	int			index;

	if (!input || !*input)
		return (NULL);
	tokens = tokenize_input(input);
	if (!tokens)
		return (NULL);
	index = 0;
	ast = parse_or_expression(tokens, &index);
	free_tokens(tokens);
	return (ast);
}
