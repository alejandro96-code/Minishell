/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 15:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/24 19:08:56 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	count_pipes(char *input)
{
    int		i;
    int		count;
    char	quote;
    int		in_quotes;

    i = -1;
    count = 0;
    quote = 0;
    in_quotes = 0;
    while (input[++i])
    {
        if (!in_quotes && (input[i] == '\"' || input[i] == '\''))
        {
            in_quotes = 1;
            quote = input[i];
        }
        else if (in_quotes && input[i] == quote)
        {
            in_quotes = 0;
            quote = 0;
        }
        else if (!in_quotes && input[i] == '|')
            count++;
    }
    return (count + 1);
}

static char	*extract_command(char *input, int start, int end)
{
    char	*cmd;
    int		len;

    len = end - start;
    cmd = malloc(sizeof(char) * (len + 1));
    if (!cmd)
        return (NULL);
    ft_strlcpy(cmd, input + start, len + 1);
    return (cmd);
}

static int	find_next_pipe(char *input, int start)
{
    int		i;
    char	quote;
    int		in_quotes;

    i = start;
    quote = 0;
    in_quotes = 0;
    while (input[i])
    {
        if (!in_quotes && (input[i] == '\"' || input[i] == '\''))
        {
            in_quotes = 1;
            quote = input[i];
        }
        else if (in_quotes && input[i] == quote)
        {
            in_quotes = 0;
            quote = 0;
        }
        else if (!in_quotes && input[i] == '|')
            return (i);
        i++;
    }
    return (i);
}

static char	**pipe_spliter(char *input)
{
    char	**result;
    int		cmd_count;
    int		i;
    int		start;
    int		end;

    cmd_count = count_pipes(input);
    result = malloc(sizeof(char *) * (cmd_count + 1));
    if (!result)
        return (NULL);
    i = 0;
    start = 0;
    while (i < cmd_count)
    {
        end = find_next_pipe(input, start);
        result[i] = extract_command(input, start, end);
        if (!result[i])
        {
            ft_free_split(result);
            return (NULL);
        }
        start = end + 1;
        i++;
    }
    return (result[i] = NULL, result);
}

static int	check_pipe_syntax_error(char *input)
{
	char	*trimmed_input;

	trimmed_input = ft_strtrim(input, " \t\n");
	if (!trimmed_input || ft_strlen(trimmed_input) == 0)
	{
		if (trimmed_input)
			free(trimmed_input);
		return (0);
	}
	if (trimmed_input[0] == '|')
	{
		free(trimmed_input);
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (1);
	}
	if (trimmed_input[ft_strlen(trimmed_input) - 1] == '|')
	{
		free(trimmed_input);
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (1);
	}
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
	*commands = pipe_spliter(input);
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
