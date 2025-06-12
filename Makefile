# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/07 20:15:32 by dgasco-g          #+#    #+#              #
#    Updated: 2025/06/12 23:05:12 by dgasco-g         ###   ########.fr        #
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
      parser/parser.c\
      parser/args.c\
      parser/logical_parser.c\
      parser/logical_executor.c\
      pipes/pipes.c\
      pipes/pipe_utils.c\
      pipes/pipe_exec.c\
      pipes/pipe_handler.c\
      redirections/redirections.c\
      signals/signals.c\
      utils/expand_variable.c\
      utils/find_user.c\
      utils/env_utils.c\
      utils/error_utils.c\
      utils/quote_utils.c\
      utils/prompt_utils.c\
      utils/input_processor.c\
      wildcards/wildcard_utils.c\
      wildcards/wildcard_match.c\
      wildcards/wildcard_expand.c\
      wildcards/wildcard_args.c\
      main.c

# Definir el directorio de objetos y crear la estructura de directorios
OBJ_DIR = obj
OBJ_SUBDIRS = $(OBJ_DIR) $(OBJ_DIR)/builtins $(OBJ_DIR)/executors $(OBJ_DIR)/parser \
              $(OBJ_DIR)/pipes $(OBJ_DIR)/redirections $(OBJ_DIR)/signals \
              $(OBJ_DIR)/utils $(OBJ_DIR)/wildcards

# Definir los objetos con la ruta del directorio obj/
OBJS = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

LIBFT = libft/libft.a

all: $(OBJ_SUBDIRS) $(NAME)

# Crear los directorios de objetos
$(OBJ_SUBDIRS):
	mkdir -p $@

# Regla de compilación modificada para colocar los objetos en obj/
$(OBJ_DIR)/%.o: %.c
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
	rm -rf $(OBJ_DIR)

fclean: clean
	make fclean -C libft
	rm -f $(NAME)
	

re: fclean all

.PHONY: all clean fclean re