NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c \
      executors/exec_cmd.c \
      builtins/cd.c \
      builtins/echo.c \
      builtins/env.c \
      builtins/exit.c \
      builtins/export.c \
      builtins/pwd.c \
      builtins/unset.c \
      

OBJ = $(SRC:.c=.o)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
