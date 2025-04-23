#include "../minishell.h"

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

// Ejecuta un comando externo
void execute_external(char **args, char **env)
{    
    pid_t pid; 
    char *path;

    path = get_path(args[0], env);
    pid = fork();
    if (pid == 0)
    {
        if (!path)
        {
            execve(args[0], args, env);
            exit(128);
        }
        // Proceso hijo: intenta ejecutar el comando
        if (execve(path, args, env) == -1) {
            perror("Error ejecutando el comando");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
        wait(NULL);
    else
        perror("Error en fork");
}
