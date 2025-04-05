NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c \
      executors/exec_cmd.c \
      parser/parser.c \
      builtins/cd.c \
      builtins/echo.c \
      builtins/env.c \
      builtins/exit.c \
      builtins/export.c \
      builtins/pwd.c \
      builtins/unset.c \
      utils/expand_variable.c \

OBJ = $(SRC:.c=.o)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
INCLUDES = -I$(LIBFT_DIR)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(NAME) $(OBJ) $(LIBFT)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

all: $(NAME)

clean:
	rm -f $(OBJ)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all
