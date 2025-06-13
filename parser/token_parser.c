/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 19:30:00 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/13 19:30:00 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Verifica si un token es un operador lógico
int	is_operator(const char *token)
{
	if (!token)
		return (0);
	return (ft_strncmp(token, "&&", 3) == 0 || ft_strncmp(token, "||", 3) == 0);
}

// Verifica si un token es un paréntesis
int	is_parenthesis(const char *token)
{
	if (!token)
		return (0);
	return (ft_strncmp(token, "(", 2) == 0 || ft_strncmp(token, ")", 2) == 0);
}

// Añade un token a la lista de tokens
void	add_token_to_list(char ***tokens, int *count, char *token)
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

// Procesa un token especial (paréntesis u operadores)
static void	process_special_token(const char *input, int *i, char ***tokens, int *count)
{
	char	*token;

	if (input[*i] == '(' || input[*i] == ')')
	{
		token = ft_substr(input, *i, 1);
		add_token_to_list(tokens, count, token);
		free(token);
		(*i)++;
	}
	else if (input[*i] == '&' && input[*i + 1] == '&')
	{
		token = ft_strdup("&&");
		add_token_to_list(tokens, count, token);
		free(token);
		*i += 2;
	}
	else if (input[*i] == '|' && input[*i + 1] == '|')
	{
		token = ft_strdup("||");
		add_token_to_list(tokens, count, token);
		free(token);
		*i += 2;
	}
}

// Procesa una palabra normal manejando comillas
static void	process_word_token(const char *input, int *i, char ***tokens, int *count)
{
	int		start;
	char	*token;
	char	quote;

	start = *i;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '(' && input[*i] != ')' && !(input[*i] == '&'
			&& input[*i + 1] == '&') && !(input[*i] == '|' && input[*i + 1] == '|'))
	{
		if (input[*i] == '"' || input[*i] == '\'')
		{
			quote = input[*i];
			(*i)++;
			while (input[*i] && input[*i] != quote)
				(*i)++;
			if (input[*i] == quote)
				(*i)++;
		}
		else
			(*i)++;
	}
	token = ft_substr(input, start, *i - start);
	add_token_to_list(tokens, count, token);
	free(token);
}

// Tokeniza la entrada en tokens individuales
char	**tokenize_input(const char *input)
{
	char	**tokens;
	int		count;
	int		i;

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
		if (input[i] == '(' || input[i] == ')' || (input[i] == '&' 
				&& input[i + 1] == '&') || (input[i] == '|' && input[i + 1] == '|'))
			process_special_token(input, &i, &tokens, &count);
		else
			process_word_token(input, &i, &tokens, &count);
	}
	return (tokens);
}