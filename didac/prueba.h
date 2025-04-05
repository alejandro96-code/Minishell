#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H
# include <stdlib.h>
# include <stdio.h>

typedef struct s_history
{
	char            *content;

	struct s_history	*next;
}	t_history;

typedef struct s_data
{
	char            *user;

}	t_data;



#endif