/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_executor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 19:10:00 by alejanr2          #+#    #+#             */
/*   Updated: 2025/06/13 19:06:14 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Construye el comando completo concatenando todos los argumentos
char	*build_full_command(t_command *cmd)
{
	char	*full_command;
	char	*temp;
	int		i;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (NULL);
	full_command = ft_strdup("");
	i = 0;
	while (i < cmd->argc && cmd->argv[i])
	{
		temp = full_command;
		if (i > 0)
			full_command = ft_strjoin(temp, " ");
		else
			full_command = ft_strdup("");
		free(temp);
		temp = full_command;
		full_command = ft_strjoin(temp, cmd->argv[i]);
		free(temp);
		i++;
	}
	return (full_command);
}

// Procesa las comillas y variables en los argumentos del comando
void	process_command_arguments(t_command *cmd, char **env, int exit_status)
{
	char	*processed;
	int		i;

	i = 0;
	while (i < cmd->argc && cmd->argv[i])
	{
		processed = process_quotes_and_variables(cmd->argv[i], env, exit_status);
		if (processed)
		{
			free(cmd->argv[i]);
			cmd->argv[i] = processed;
		}
		i++;
	}
	cmd->argv = expand_wildcards_in_args(cmd->argv, &cmd->argc);
}

// Ejecuta un comando builtin con manejo de redirecciones
int	execute_builtin_command(t_command *cmd, char ***env, int saved_stdin, int saved_stdout)
{
	int	exit_status;

	handle_redirections(&cmd->argv, *env);
	exit_status = 0;
	execute_builtin(cmd->argv, env, &exit_status);
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	return (exit_status);
}

// Ejecuta un comando externo en un proceso hijo
int	execute_external_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;
	int		exit_status;

	pid = fork();
	if (pid == 0)
	{
		handle_redirections(&cmd->argv, *env);
		exit_status = execute_external(cmd->argv, *env);
		exit(exit_status);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		else
			exit_status = 1;
	}
	else
	{
		perror("fork");
		exit_status = 1;
	}
	return (exit_status);
}
