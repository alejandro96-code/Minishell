/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 00:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/25 00:00:00 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_pipes(char *input)
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

char	*extract_command(char *input, int start, int end)
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

int	find_next_pipe(char *input, int start)
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
