/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 15:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/19 19:12:30 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	check_pipe_syntax_error(char *input)
{
	char	*trimmed_input;

	trimmed_input = ft_strtrim(input, " \t\n");
	if (trimmed_input && ft_strlen(trimmed_input) > 0
		&& trimmed_input[ft_strlen(trimmed_input) - 1] == '|')
	{
		free(trimmed_input);
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (1);
	}
	if (trimmed_input)
		free(trimmed_input);
	return (0);
}

static int	validate_commands(char ***commands)
{
	int		count;
	char	*trimmed;

	count = 0;
	while ((*commands)[count])
	{
		trimmed = ft_strtrim((*commands)[count], " \t\n");
		if (!trimmed || ft_strlen(trimmed) == 0)
		{
			if (trimmed)
				free(trimmed);
			ft_free_split(*commands);
			*commands = NULL;
			ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
			return (-1);
		}
		free(trimmed);
		count++;
	}
	return (count);
}

int	split_and_validate_commands(char *input, char ***commands)
{
	if (check_pipe_syntax_error(input))
	{
		*commands = NULL;
		return (-1);
	}
	*commands = ft_split(input, '|');
	return (validate_commands(commands));
}

void	handle_child_process(int i, int cmd_count, int pipefd[2],
		int *prev_pipe)
{
	if (i > 0)
	{
		dup2(*prev_pipe, STDIN_FILENO);
		close(*prev_pipe);
	}
	if (i < cmd_count - 1)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
	}
}
