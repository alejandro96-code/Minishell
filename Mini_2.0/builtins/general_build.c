/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general_build.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/24 18:22:54 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_builtin(char *cmd)
{
	return (!strcmp(cmd, "cd") || !strcmp(cmd, "echo") || !strcmp(cmd, "pwd")
		|| !strcmp(cmd, "export") || !strcmp(cmd, "unset") || !strcmp(cmd,
			"env") || !strcmp(cmd, "exit"));
}

int	execute_builtin(char **args, char ***env)
{
	if (!strcmp(args[0], "cd"))
		return (builtin_cd(args, *env));
	if (!strcmp(args[0], "echo"))
		return (builtin_echo(args, *env));
	if (!strcmp(args[0], "pwd"))
		return (builtin_pwd(*env));
	if (!strcmp(args[0], "export"))
		return (builtin_export(args, env));
	if (!strcmp(args[0], "unset"))
		return (builtin_unset(args, env));
	if (!strcmp(args[0], "env"))
		return (builtin_env(*env));
	if (!strcmp(args[0], "exit"))
		return (builtin_exit(args));
	return (1);
}
/* 
void	execute_external(char **args, char **env)
{
	pid_t	pid;

	(void)env;
	pid = fork();
	if (pid == 0)
	{
		reset_signal_handlers();
		if (execvp(args[0], args) == -1)
		{
			perror("Error ejecutando el comando");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid > 0)
	{
		reset_signal_handlers();
		wait(NULL);
	}
	else
	{
		perror("Error en fork");
	}
} */


void execute_external(char **args, char **env)
{    
    pid_t pid; 
    char *path;

    path = get_path(args[0], env);
    pid = fork();
    if (pid == 0)
    {
		reset_signal_handlers();
        if (!path)
        {
            execve(args[0], args, env);
			perror("Error ejecutando el comando");
            exit(EXIT_FAILURE);
        }
		else
		{
			// Proceso hijo: intenta ejecutar el comando
     		execve(path, args, env);
			perror("Error ejecutando el comando");
			exit(EXIT_FAILURE);
		}
    }
    else if (pid > 0)
        wait(NULL);
    else
	{
		perror("Error en fork");
	}        
	free(path);
}

