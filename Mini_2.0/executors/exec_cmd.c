/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/15 20:51:45 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_split(char **split)
{
	int	cont;

	if (!split)
		return ;
	cont = 0;
	while (split[cont])
	{
		free(split[cont]);
		cont++;
	}
	free(split);
}

char	*get_path(char *cmd, char **envp)
{
	char	**paths;
	char	*full_path;

	paths = extract_paths(envp);
	full_path = NULL;
	if (!paths)
		return (NULL);
	full_path = search_in_paths(paths, cmd);
	ft_free_split(paths);
	return (full_path);
}

char	**extract_paths(char **envp)
{
	int	i;

	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (NULL);
	return (ft_split(envp[i] + 5, ':'));
}

char	*search_in_paths(char **paths, char *cmd)
{
	int		i;
	char	*temp;
	char	*full_path;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, X_OK) == 0)
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
