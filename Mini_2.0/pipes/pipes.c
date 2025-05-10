/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/10 13:22:47 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Divide el input por '|',
// guarda el resultado en commands y retorna num de comandos
int	count_commands_and_split(char *input, char ***commands)
{
	int	count;

	*commands = ft_split(input, '|');
	count = 0;
	while ((*commands)[count])
		count++;
	return (count);
}

// Crea el pipe si es necesario y realiza fork.
// Si es el hijo,configura entradas/salidas.
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

void	setup_pipes_and_fork(int i, int cmd_count, int pipefd[2],
		int *prev_pipe)
{
	pid_t	pid;

	if (i < cmd_count - 1 && pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0)
		handle_child_process(i, cmd_count, pipefd, prev_pipe);
}

// Divide un comando en argumentos, elimina comillas y maneja redirecciones
char	**parse_args_and_handle(char *command, char **env)
{
	char	**args;
	int		i;

	args = ft_split(command, ' ');
	i = 0;
	while (args[i])
	{
		args[i] = remove_quotes(args[i]);
		i++;
	}
	handle_redirections(&args, env);
	return (args);
}

// Ejecuta el comando en el hijo: builtin o execve, con manejo de errores
void	child_exec_or_builtin(char *command, char ***env)
{
	char	**args;
	int		num_args;
	char	*cmd_path;

	args = parse_args_and_handle(command, *env);
	num_args = 0;
	while (args && args[num_args])
		num_args++;
	args = expand_wildcards_in_args(args, &num_args);
	if (args[0])
	{
		if (is_builtin(args[0]))
			exit(execute_builtin(args, env));
		cmd_path = get_path(args[0], *env);
		if (!cmd_path)
		{
			fprintf(stderr, "%s: command not found\n", args[0]);
			exit(EXIT_FAILURE);
		}
		execve(cmd_path, args, *env);
		perror("execve");
		free(cmd_path);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

// Ejecuta una línea de comandos en pipeline:
// fork,pipes y espera de procesos hijos
static void	handle_pipeline_iteration(int i, int cmd_count, char **commands,
		char ***env)
{
	int			pipefd[2];
	static int	prev_pipe = STDIN_FILENO;

	setup_pipes_and_fork(i, cmd_count, pipefd, &prev_pipe);
	if (fork() == 0)
	{
		setup_pipes_and_fork(i, cmd_count, pipefd, &prev_pipe);
		child_exec_or_builtin(commands[i], env);
	}
	if (i > 0)
		close(prev_pipe);
	if (i < cmd_count - 1)
	{
		prev_pipe = pipefd[0];
		close(pipefd[1]);
	}
}

int	execute_pipeline(char *input, char **env)
{
	char	**commands;
	int		cmd_count;
	int		status;
	int		i;

	i = -1;
	cmd_count = count_commands_and_split(input, &commands);
	if (cmd_count == 1)
		return (process_input(commands[0], &env), ft_free_split(commands), 0);
	while (++i < cmd_count)
		handle_pipeline_iteration(i, cmd_count, commands, &env);
	i = 0;
	while (i++ < cmd_count && wait(&status) != -1)
		;
	ft_free_split(commands);
	return (0);
}
