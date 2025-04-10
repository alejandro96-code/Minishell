#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include "prueba.h"

int	main(int argc, char **argv, char **env)
{
	char *input;
	char *user;

	user = find_user(env);
	while (1)
	{
		input = readline(user);
		if (input && *input)
			add_history(input);
		printf("Lo que introdujiste: %s\n", input);
		free(input);
	}
	return (0);
}
