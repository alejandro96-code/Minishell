/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 19:35:00 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/13 20:20:00 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Crea un nuevo nodo del AST
t_ast_node	*create_ast_node(t_operator_type op, t_command *cmd)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->operator= op;
	node->command = cmd;
	node->left = NULL;
	node->right = NULL;
	node->in_parentheses = 0;
	return (node);
}