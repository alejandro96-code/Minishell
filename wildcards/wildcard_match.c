/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_match.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:40:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 14:40:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	match_pattern(const char *pattern, const char *filename)
{
	if (!pattern || !filename)
		return (0);
	if (!contains_wildcard(pattern))
		return (ft_strncmp(pattern, filename, ft_strlen(pattern) + 1) == 0);
	return (match_pattern_with_wildcard(pattern, filename));
}

static int	should_skip_part(int i, const char *pattern, char **parts)
{
	if (i == 0 && pattern[0] != '*')
		return (1);
	if (ft_strlen(parts[i]) == 0)
		return (1);
	return (0);
}

static char	*find_part_in_filename(const char *filename, size_t pos, char *part)
{
	return (strstr(filename + pos, part));
}

static void	update_position(char *found, const char *filename,
		char *part, size_t *pos)
{
	*pos = (found - filename) + ft_strlen(part);
}

int	check_pattern_middle(const char *pattern, const char *filename,
		char **parts, size_t *pos)
{
	int		i;
	char	*found;

	i = 0;
	while (parts[i])
	{
		if (should_skip_part(i, pattern, parts))
		{
			i++;
			continue ;
		}
		found = find_part_in_filename(filename, *pos, parts[i]);
		if (!found)
			return (0);
		update_position(found, filename, parts[i], pos);
		i++;
	}
	return (1);
}

int	count_matching_files(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	count = 0;
	dir = opendir(".");
	if (!dir)
		return (0);
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.' && pattern[0] != '.')
			continue ;
		if (match_pattern(pattern, entry->d_name))
			count++;
	}
	closedir(dir);
	return (count);
}
