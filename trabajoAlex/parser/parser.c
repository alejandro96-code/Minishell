#include "../minishell.h"
#include "../libft/libft.h"

// Función para verificar si el comando es un builtin (puedes agregar más)
int is_builtin_command(const char *cmd) {
    return (strcmp(cmd, "echo") == 0 || strcmp(cmd, "cd") == 0 || strcmp(cmd, "pwd") == 0);
}

// Función que procesa la entrada y genera la estructura t_command
t_command *parse_input(const char *input) {
    t_command *cmd = malloc(sizeof(t_command));
    if (!cmd)
        return NULL;

    // Separar el input en tokens usando ft_split de libft
    cmd->argv = ft_split(input, ' ');

    // Calcular el número de argumentos (argc)
    int i = 0;
    while (cmd->argv[i] != NULL) {
        i++;
    }
    cmd->argc = i;

    // Verificar si el comando es un builtin
    cmd->is_builtin = is_builtin_command(cmd->argv[0]);

    return cmd;
}
