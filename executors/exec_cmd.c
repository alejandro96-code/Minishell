/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:35:18 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*construct_full_path(char *dir, char *cmd)
{
	char	*temp;
	char	*full_path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	full_path = ft_strjoin(temp, cmd);
	free(temp);
	return (full_path);
}

static void	free_paths(char **paths)
{
	int	i;

	if (!paths)
		return ;
	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}

void	ft_free_split(char **split)
{
	int	i;

	i = 0;
	if (!split)
		return ;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

char	*get_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path;

	if (cmd[0] == '/' || cmd[0] == '.')
		return (ft_strdup(cmd));
	paths = extract_paths(envp);
	if (!paths)
		return (NULL);
	path = search_in_paths(paths, cmd);
	ft_free_split(paths);
	return (path);
}

char	**extract_paths(char **envp)
{
	int		i;
	char	*path_env;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path_env = envp[i] + 5;
			return (ft_split(path_env, ':'));
		}
		i++;
	}
	return (NULL);
}

char	*search_in_paths(char **paths, char *cmd)
{
	int		i;
	char	*full_path;

	i = 0;
	while (paths[i])
	{
		full_path = construct_full_path(paths[i], cmd);
		if (full_path && access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

void	execute_command(char *cmd_line, char **envp)
{
	char	**argv;
	char	*cmd_path;
	pid_t	pid;

	argv = ft_split(cmd_line, ' ');
	cmd_path = get_path(argv[0], envp);
	if (!cmd_path)
	{
		printf("%s: command not found\n", argv[0]);
		ft_free_split(argv);
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		execve(cmd_path, argv, envp);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else
		wait(NULL);
	free(cmd_path);
	ft_free_split(argv);
}

// Actualizar la función execute_external para manejar señales
void	execute_external(char **args, char **env)
{
	char	*cmd_path;
	pid_t	pid;
	int		status;

	if (!args || !args[0])
		return ;
	
	// Check for direct path
	if (ft_strchr(args[0], '/'))
	{
		if (access(args[0], X_OK) == 0)
			cmd_path = ft_strdup(args[0]);
		else
		{
			printf("minishell: %s: No such file or directory\n", args[0]);
			return ;
		}
	}
	else
	{
		cmd_path = get_path(args[0], env);
		if (!cmd_path)
		{
			printf("minishell: %s: command not found\n", args[0]);
			return ;
		}
	}
	
	pid = fork();
	if (pid == 0)
	{
		// En el proceso hijo, restaurar señales por defecto
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		
		execve(cmd_path, args, env);
		perror("execve");
		exit(127);
	}
	else if (pid > 0)
	{
		// En el proceso padre, cambiar temporalmente el manejo de señales
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		
		waitpid(pid, &status, 0);
		
		// Verificar si el proceso hijo fue terminado por una señal
		if (WIFSIGNALED(status))
		{
			int sig = WTERMSIG(status);
			if (sig == SIGINT)
			{
				printf("\n");
				g_signal_received = SIGINT;
			}
			else if (sig == SIGQUIT)
			{
				printf("Quit (core dumped)\n");
				g_signal_received = SIGQUIT;
			}
		}
		
		// Restaurar el manejo de señales del minishell
		setup_signal_handlers();
	}
	else
	{
		perror("fork");
	}
	
	free(cmd_path);
}

char	*find_command_path(char *cmd, char **env)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (cmd[0] == '/' || cmd[0] == '.')
		return (ft_strdup(cmd));
	path_env = get_env_var("PATH", env);
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = construct_full_path(paths[i], cmd);
		if (full_path && access(full_path, X_OK) == 0)
		{
			free_paths(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_paths(paths);
	return (NULL);
}
