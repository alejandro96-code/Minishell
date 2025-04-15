# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/07 20:15:32 by dgasco-g          #+#    #+#              #
#    Updated: 2025/04/15 20:00:30 by alejandro        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = cc
CFLAGS = -Wall -Werror -Wextra -g -O3 
RM = rm -f

SRC =	builtins/cd.c\
      builtins/echo.c\
      builtins/env.c\
      builtins/exit.c\
      builtins/export.c\
      builtins/general_build.c\
      builtins/pwd.c\
      builtins/unset.c\
      executors/exec_cmd.c\
      parser/parser.c\
      pipes/pipes.c\
      redirections/redirections.c\
      utils/autocomplete.c\
      utils/expand_variable.c\
      utils/find_user.c\
      wildcards/wildcards.c\
      main.c

OBJS = $(SRC:.c=.o)

LIBFT = libft/libft.a

all: $(NAME)

%.o: %.c
	$(CC) -I src/ $(CFLAGS) -c $^ -o $@

$(LIBFT):
	$(MAKE) -C libft 
	
$(PRINTF):
	$(MAKE) -C printf 

$(NAME): $(OBJS) $(LIBFT) 
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) libft/libft.a -lreadline -lhistory


sanitize:
	$(eval CFLAGS+=-fsanitize=address,leak -g3)

sani: sanitize re

clean:
	make clean -C libft
	rm -f $(OBJS)

fclean: clean
	make fclean -C libft
	rm -f $(NAME)
	

re: fclean all

.PHONY: all clean fclean re