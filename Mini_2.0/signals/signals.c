/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:08:09 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Manejador global para la señal SIGINT (Ctrl+C)
void	sigint_handler(int sig)
{
	int	ret;

	(void)sig;
	ret = write(STDOUT_FILENO, "\n", 1);
	(void)ret;
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

// Manejador global para la señal SIGQUIT (Ctrl+\)
void	sigquit_handler(int sig)
{
	(void)sig;
}

// Configura los manejadores de señales
void	setup_signal_handlers(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = sigint_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	signal(SIGINT, sigint_handler);
	sa_quit.sa_handler = sigquit_handler;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

// Configuración de señales para procesos hijo
void	setup_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

// Restaura el comportamiento por defecto de las señales
void	reset_signal_handlers(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
