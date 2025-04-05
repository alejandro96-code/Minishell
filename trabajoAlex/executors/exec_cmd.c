#include "../minishell.h"
#include "../libft/libft.h"

char	*get_path(char *cmd, char **envp)
{
	char	**paths;
	char	*full_path;
	char	*temp;
	int		cont = 0;

	while (envp[cont] && strncmp(envp[cont], "PATH=", 5) != 0)
		cont++;
	if (!envp[cont])
		return (NULL);
	paths = ft_split(envp[cont] + 5, ':');
	cont = 0;
	while (paths[cont])
	{
		temp = ft_strjoin(paths[cont], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, X_OK) == 0)
		{
			ft_free_split(paths);
			return (full_path);
		}
		free(full_path);
		cont++;
	}
	ft_free_split(paths);
	return (NULL);
}

void	execute_command(char *cmd_line, char **envp)
{
	char	**argv = ft_split(cmd_line, ' ');
	char	*cmd_path = get_path(argv[0], envp);
	pid_t	pid;

	if (!cmd_path)
	{
		printf("%s: command not found\n", argv[0]);
		ft_free_split(argv);
		return;
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
