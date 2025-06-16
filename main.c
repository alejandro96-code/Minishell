/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/15 21:14:40 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_input(char *input, char ***env, int *exit_status)
{
	if (*input)
	{
		add_history(input);
		process_input(input, env, exit_status);
	}
	else
		free(input);
}

static int	main_loop(char ***env)
{
	char	*input;
	char	*prompt;
	int		exit_status;

	exit_status = 0;
	while (1)
	{
		g_signal_received = 0;
		prompt = get_prompt(*env);
		input = readline(prompt);
		free(prompt);
		if (!input)
		{
			break ;
		}
		handle_input(input, env, &exit_status);
	}
	return (exit_status);
}

int	main(int argc, char **argv, char **envp)
{
	char	**env;
	int		final_exit_status;

	(void)argc;
	(void)argv;
	env = copy_env_array(envp);
	if (!env)
		return (1);
	setup_signal_handlers();
	final_exit_status = main_loop(&env);
	cleanup_and_exit(env);
	return (final_exit_status);
}
