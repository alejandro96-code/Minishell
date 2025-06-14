/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/14 13:05:04 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*build_prompt_part1(char *username)
{
	char	*prompt;
	char	*temp;

	prompt = ft_strjoin(YELLOW, username);
	temp = prompt;
	prompt = ft_strjoin(temp, RED);
	free(temp);
	return (prompt);
}

static char	*build_prompt_part2(char *prompt, char *cwd)
{
	char	*temp;

	temp = prompt;
	prompt = ft_strjoin(temp, cwd);
	free(temp);
	temp = prompt;
	prompt = ft_strjoin(temp, RESET);
	free(temp);
	temp = prompt;
	prompt = ft_strjoin(temp, ": ");
	free(temp);
	return (prompt);
}

char	*get_prompt(char **env)
{
	char	*username;
	char	cwd[1024];
	char	*prompt;

	username = find_user(env);
	if (!username)
		return (NULL);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		free(username);
		return (NULL);
	}
	prompt = build_prompt_part1(username);
	free(username);
	if (!prompt)
		return (NULL);
	prompt = build_prompt_part2(prompt, cwd);
	return (prompt);
}
