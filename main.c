/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/10 18:47:51 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Variable global para el exit status
int	g_exit_status = 0;

static void	handle_input(char *input, char ***env)
{
	if (*input)
	{
		add_history(input);
		process_input(input, env);
	}
	else
		free(input);
}

static void	main_loop(char ***env)
{
	char	*input;
	char	*prompt;

	while (1)
	{
		g_signal_received = 0;
		prompt = get_prompt(*env);
		input = readline(prompt);
		free(prompt);
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		handle_input(input, env);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char	**env;

	(void)argc;
	(void)argv;
	env = copy_env(envp);
	if (!env)
		return (1);
	setup_signal_handlers();
	main_loop(&env);
	cleanup_and_exit(env);
	return (0);
}
