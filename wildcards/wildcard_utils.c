/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:40:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	contains_wildcard(const char *str)
{
	if (!str)
		return (0);
	return (ft_strchr(str, '*') != NULL);
}

static void	free_pattern_parts(char **parts)
{
	int	i;

	i = 0;
	while (parts[i])
	{
		free(parts[i]);
		i++;
	}
	free(parts);
}

static int	check_pattern_start(const char *pattern, const char *filename,
		char **parts, size_t *pos)
{
	if (pattern[0] != '*' && parts[0] && ft_strlen(parts[0]) > 0)
	{
		if (ft_strncmp(filename, parts[0], ft_strlen(parts[0])) != 0)
			return (0);
		*pos = ft_strlen(parts[0]);
	}
	return (1);
}

static int	check_pattern_end(const char *pattern, size_t filename_len,
		char **parts, size_t pos)
{
	int	i;

	i = 0;
	while (parts[i])
		i++;
	if (pattern[ft_strlen(pattern) - 1] != '*' && parts[i - 1]
		&& ft_strlen(parts[i - 1]) > 0)
	{
		if (pos != filename_len)
			return (0);
	}
	return (1);
}

int	match_pattern_with_wildcard(const char *pattern, const char *filename)
{
	char	**parts;
	int		match;
	size_t	pos;

	parts = ft_split(pattern, '*');
	if (!parts)
		return (0);
	pos = 0;
	match = check_pattern_start(pattern, filename, parts, &pos);
	if (match)
		match = check_pattern_middle(pattern, filename, parts, &pos);
	if (match)
		match = check_pattern_end(pattern, ft_strlen(filename), parts, pos);
	free_pattern_parts(parts);
	return (match);
}
