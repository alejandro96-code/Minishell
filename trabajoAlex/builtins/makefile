NAME = minishell

CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c \
      builtins/builtin_cd.c \
      builtins/builtin_echo.c \
      builtins/builtin_env.c \
      builtins/builtin_exit.c \
      builtins/builtin_export.c \
      builtins/builtin_pwd.c \
      builtins/builtin_unset.c

OBJ = $(SRC:.c=.o)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
