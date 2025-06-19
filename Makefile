# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/07 20:15:32 by dgasco-g          #+#    #+#              #
#    Updated: 2025/06/18 19:50:58 by alejandro        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CC = cc
CFLAGS =  -Wall -Wextra -Werror -g -O3 
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
      executors/exec_utils.c\
      executors/exec_path.c\
      executors/exec_process.c\
      parser/command_parser.c\
      parser/argument_extraction.c\
      parser/argument_splitter.c\
      parser/argument_utils.c\
      parser/parser_utils.c\
      pipes/pipe_utils.c\
      pipes/pipe_exec.c\
      pipes/pipe_handler.c\
      redirections/redirections.c\
      redirections/redirections_handler.c\
      redirections/redirections_basic.c\
      redirections/redirections_heredoc.c\
      signals/signals.c\
      utils/expand_buffer.c\
      utils/expand_parsing.c\
      utils/expand_helpers.c\
      utils/expand_processing.c\
      utils/expand_variable.c\
      utils/find_user.c\
      utils/env_utils.c\
      utils/quote_utils.c\
      utils/prompt_utils.c\
      utils/input_processor.c\
      cleanup/cleanup.c\
      main.c

OBJS = $(SRC:.c=.o)

LIBFT = libft/libft.a

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

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
	find . -name "*.o" -type f -delete

fclean: clean
	make fclean -C libft
	rm -f $(NAME)
	

re: fclean all

.PHONY: all clean fclean re