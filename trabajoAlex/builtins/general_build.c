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
    if (!args || !*args || !env || !*env)
        return 1;
        
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
    pid_t pid = fork();
    (void)env; // usar en el execve 
    if (pid == 0) {
        // Proceso hijo: intenta ejecutar el comando
        if (execvp(args[0], args) == -1) {
            perror("Error ejecutando el comando");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // Proceso padre: espera que termine el hijo
        wait(NULL);
    } else {
        // Error al hacer fork
        perror("Error en fork");
    }
}