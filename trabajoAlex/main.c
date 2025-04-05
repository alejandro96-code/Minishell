#include "minishell.h"
#include "libft/libft.h"

// Detecta si es un builtin
int is_builtin(char *cmd)
{
    return (
        !strcmp(cmd, "cd") ||
        !strcmp(cmd, "echo") ||
        !strcmp(cmd, "pwd") ||
        !strcmp(cmd, "export") ||
        !strcmp(cmd, "unset") || 
        !strcmp(cmd, "env") ||
        !strcmp(cmd, "exit")
    );
}

// Ejecuta el builtin correspondiente
int execute_builtin(char **args, char ***env)
{
    if (!strcmp(args[0], "cd"))
        return builtin_cd(args, *env);
    if (!strcmp(args[0], "echo"))
        return builtin_echo(args, *env);
    if (!strcmp(args[0], "pwd"))
        return builtin_pwd(*env);
    if (!strcmp(args[0], "export"))
        return builtin_export(args, env);
    if (!strcmp(args[0], "unset"))
        return builtin_unset(args, env);
    if (!strcmp(args[0], "env"))
        return builtin_env(*env);
    if (!strcmp(args[0], "exit"))
        return builtin_exit(args);
    return (1);
}

// Copia el envp al entorno local
char **copy_env(char **envp)
{
    int cont = 0;
    while (envp[cont])
        cont++;
    char **copy = malloc((cont + 1) * sizeof(char *));
    cont = 0;
    while (envp[cont])
    {
        copy[cont] = strdup(envp[cont]);
        cont++;
    }
    copy[cont] = NULL;
    return copy;
}

int main(int argc, char **argv, char **envp)
{
    char *input = NULL;
    size_t len = 0;
    char **args;
    char **env = copy_env(envp);

    (void)argc;
    (void)argv;

    printf("Minishell builtins test mode. Ctrl+C to exit.\n");
    while (1)
    {
        printf("> ");
        if (getline(&input, &len, stdin) == -1)
            break;
        
        // Utilizar ft_split de libft para separar el input en tokens
        args = ft_split(input, ' ');

        if (args[0])
        {
            if (is_builtin(args[0]))
                execute_builtin(args, &env);
            else
                printf("Comando no encontrado: %s\n", args[0]);
        }

        // Liberar la memoria de los argumentos
        int cont = 0;
        while (args[cont])
            free(args[cont++]);
        free(args);
    }
    free(input);

    // Liberar la memoria del entorno
    int cont = 0;
    while (env[cont])
        free(env[cont++]);
    free(env);

    return 0;
}