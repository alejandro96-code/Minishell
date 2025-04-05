#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include "prueba.h"

char *find_user(char **env)
{
	char	*path_value;
	int		i;

	path_value = NULL;
	i = 0;
	while (env[i] != NULL)
	{
		if (ft_strncmp(env[i], "USER=", 5) == 0)
		{
			path_value = env[i] + 5;
			break ;
		}
		i++;
	}
	
}

int	main(int argc, char **argv, char **env)
{
	char *input;
	char *user;

	user = find_user(env);
	while (1)
	{
		input = readline("Introduce algo: ");
		if (input && *input)
			add_history(input);
		printf("Lo que introdujiste: %s\n", input);
		free(input);
	}
	return (0);
}
