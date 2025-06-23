# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/07 20:15:32 by dgasco-g          #+#    #+#              #
#    Updated: 2025/06/23 15:35:53 by alejanr2         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colores
YELLOW = \033[33m
GREEN = \033[32m

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
	@$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	@echo "$(YELLOW)Libft compilando..."
	@$(MAKE) -C libft --no-print-directory
	@echo "$(GREEN)Libft compilada!"
	
$(PRINTF):
	@$(MAKE) -C printf --no-print-directory

$(NAME): $(OBJS) $(LIBFT) 
	@echo "$(YELLOW)Minishell compilando..."
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) libft/libft.a -lreadline -lhistory
	@echo "$(GREEN)Minishell compilada!"

sanitize:
	$(eval CFLAGS+=-fsanitize=address,leak -g3)

sani: sanitize re

clean:
	@echo "$(YELLOW)Libft y minishell limpiando..."
	@make clean -C libft --no-print-directory
	@find . -name "*.o" -type f -delete
	@echo "$(GREEN)Libft y minishell limpiadas!"

fclean: clean
	make fclean -C libft
	rm -f $(NAME)

run_test: re
		chmod +x test/launch.sh
		./test/launch.sh
val: re
		valgrind -s --leak-check=full --show-leak-kinds=all ./minishell
	
re: fclean all

.PHONY: all clean fclean re run_test val