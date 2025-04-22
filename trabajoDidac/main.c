#include "minishell.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
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

// Ejecuta un comando externo
void execute_external(char **args, char **env)
{
    pid_t pid = fork();
    (void)env; // usar en el execve 
    if (pid == 0)
    {
        // Proceso hijo: intenta ejecutar el comando
        if (execvp(args[0], args) == -1)
        {
            perror("Error ejecutando el comando");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        // Proceso padre: espera que termine el hijo
        wait(NULL);
    }
    else
    {
        // Error al hacer fork
        perror("Error en fork");
    }
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
        copy[cont] = ft_strdup(envp[cont]);
        cont++;
    }
    copy[cont] = NULL;
    return copy;
}

// Función para eliminar comillas del inicio y final del string si existen
char *remove_quotes(char *str)
{
    if (!str)
        return NULL;

    size_t len = ft_strlen(str);

    // Si el string tiene comillas al inicio y al final, las eliminamos
    if (len >= 2 && ((str[0] == '"' && str[len-1] == '"') || 
                     (str[0] == '\'' && str[len-1] == '\'')))
    {
        // Crear una copia sin las comillas
        char *result = malloc(len - 1); // -2 por las comillas, +1 por el null terminator
        if (!result)
            return str; // Si falla, devolver el original
        
        ft_strlcpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
        
        free(str); // Liberar el string original
        return result;
    }
    
    // Si las comillas están desbalanceadas, no hacemos nada
    return str;
}

// Función para limpiar las comillas del input completo
char *clean_input(char *input)
{
    if (!input)
        return NULL;

    size_t len = strlen(input);
    char *cleaned_input = malloc(len + 1);  // Almacenamos el resultado final
    size_t j = 0;

    for (size_t i = 0; i < len; i++) {
        if (input[i] == '"' || input[i] == '\'') {
            // Ignoramos las comillas
            continue;
        }
        cleaned_input[j++] = input[i];  // Copiamos el carácter sin comillas
    }

    cleaned_input[j] = '\0';  // Finalizamos la cadena
    return (cleaned_input);
}

// Función para imprimir el prompt con colores
char	*get_prompt(char ** env)
{
	char *text;
	char cwd[1024];

	text = find_user(env);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return (free (text), NULL);
	}
	text = ft_strjoin_s1_free(text, cwd);
    text = ft_strjoin_s1_free(text, ": ");
    return (text);
}

// Función principal
int main(int argc, char **argv, char **envp)
{
    char *input = NULL;                     
    char **args;
    char **env = copy_env(envp);
    char *promt;

    (void)argc;
    (void)argv;

    printf("Minishell builtins test mode. Ctrl+C to exit.\n");
    while (1)
    {
        promt = get_prompt(env);
		input = readline(promt);
        free(promt);
		if (input && *input)
			add_history(input);
        char *cleaned_input = clean_input(input);
        free(input);

        // Utilizar ft_split de libft para separar el input en tokens
        args = ft_split(cleaned_input, ' ');

        // Procesar las comillas en los argumentos
        if (args) {
            int i = 0;
            while (args[i]) {
                args[i] = remove_quotes(args[i]);  // Eliminar comillas de cada argumento
                i++;
            }
        }

        if (args && args[0])
        {
            if (is_builtin(args[0]))
                execute_builtin(args, &env);
            else
                execute_external(args, env);  // Ejecutar comando externo
        }

        // Liberar la memoria de los argumentos
        int cont = 0;
        while (args && args[cont])
            free(args[cont++]);
        free(args);
        free(cleaned_input);
    }
    free(input);

    // Liberar la memoria del entorno
    int cont = 0;
    while (env[cont])
        free(env[cont++]);
    free(env);

    return 0;
}
