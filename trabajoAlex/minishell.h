#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>


typedef struct s_command {
    char **argv;     // Array de tokens (argumentos)
    int argc;        // Número de argumentos
    int is_builtin;  // Indica si es un comando builtin (1 si lo es, 0 si no)
} t_command;

// Funciones de ejecución
void	execute_command(char *cmd_line, char **envp);
char	*get_path(char *cmd, char **envp);

//parseo de comandos
int is_builtin_command(const char *cmd);
t_command *parse_input(const char *input);

// Utils
char	**ft_split(char const *s, char c);
char	*ft_strjoin(char const *s1, char const *s2);
void	ft_free_split(char **arr);


// funciones de builtin
int builtin_echo(char **args);
int builtin_cd(char **args, char **env);
int builtin_pwd(char **env);
int builtin_export(char **args, char ***env);
int builtin_unset(char **args, char ***env);
int builtin_env(char **env);
int builtin_exit(char **args);

#endif