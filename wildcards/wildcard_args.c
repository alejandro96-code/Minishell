/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:40:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	free_original_args(char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		free(args[i]);
		i++;
	}
	free(args);
}

static void	copy_expanded_to_new_args(char **new_args, char **expanded,
		int num_expanded, int *total_expanded)
{
	int	j;

	j = 0;
	while (j < num_expanded)
	{
		new_args[*total_expanded] = expanded[j];
		(*total_expanded)++;
		j++;
	}
}

static void	process_wildcard_arg(char **new_args, char *arg,
		int *total_expanded)
{
	int		num_expanded;
	char	**expanded;

	num_expanded = 0;
	expanded = expand_wildcards(arg, &num_expanded);
	if (expanded)
	{
		copy_expanded_to_new_args(new_args, expanded, num_expanded,
			total_expanded);
		free(expanded);
	}
	else
	{
		new_args[*total_expanded] = ft_strdup(arg);
		(*total_expanded)++;
	}
}

static void	process_regular_arg(char **new_args, char *arg, int *total_expanded)
{
	new_args[*total_expanded] = ft_strdup(arg);
	(*total_expanded)++;
}

char	**expand_wildcards_in_args(char **args, int *num_args)
{
	if (!args || !*args)
		return (args);
	return (process_args_with_wildcards(args, num_args));
}

char	**process_args_with_wildcards(char **args, int *num_args)
{
	char	**new_args;
	int		total_expanded;
	int		i;

	new_args = malloc(1024 * sizeof(char *));
	if (!new_args)
		return (args);
	total_expanded = 0;
	i = 0;
	while (args[i] != NULL)
	{
		if (contains_wildcard(args[i]))
			process_wildcard_arg(new_args, args[i], &total_expanded);
		else
			process_regular_arg(new_args, args[i], &total_expanded);
		i++;
	}
	new_args[total_expanded] = NULL;
	*num_args = total_expanded;
	free_original_args(args);
	return (new_args);
}
