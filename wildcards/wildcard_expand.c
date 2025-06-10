/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_expand.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:40:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	**create_single_arg_result(const char *arg, int *num_expanded)
{
	char	**result;

	result = malloc(2 * sizeof(char *));
	if (!result)
		return (NULL);
	result[0] = ft_strdup(arg);
	result[1] = NULL;
	*num_expanded = 1;
	return (result);
}

static char	**create_single_result(const char *arg, int *num_expanded)
{
	char	**result;

	result = malloc(2 * sizeof(char *));
	if (!result)
		return (NULL);
	result[0] = ft_strdup(arg);
	result[1] = NULL;
	*num_expanded = 1;
	return (result);
}

static char	**allocate_result_array(int capacity)
{
	char	**result;

	result = malloc((capacity + 1) * sizeof(char *));
	return (result);
}

static int	fill_matching_files(char **result, const char *arg, int capacity)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	count = 0;
	dir = opendir(".");
	if (!dir)
		return (-1);
	while ((entry = readdir(dir)) != NULL && count < capacity)
	{
		if (entry->d_name[0] == '.' && arg[0] != '.')
			continue ;
		if (match_pattern(arg, entry->d_name))
		{
			result[count] = ft_strdup(entry->d_name);
			count++;
		}
	}
	result[count] = NULL;
	closedir(dir);
	return (count);
}

char	**expand_wildcards(const char *arg, int *num_expanded)
{
	if (!contains_wildcard(arg))
		return (create_single_arg_result(arg, num_expanded));
	return (expand_wildcard_matches(arg, num_expanded));
}

char	**expand_wildcard_matches(const char *arg, int *num_expanded)
{
	char	**result;
	int		capacity;
	int		count;

	capacity = count_matching_files(arg);
	if (capacity == 0)
		return (create_single_arg_result(arg, num_expanded));
	result = allocate_result_array(capacity);
	if (!result)
		return (NULL);
	count = fill_matching_files(result, arg, capacity);
	if (count == -1)
	{
		free(result);
		return (NULL);
	}
	*num_expanded = count;
	return (result);
}
