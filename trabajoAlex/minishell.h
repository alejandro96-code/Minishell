#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>

// Funciones de ejecución
void	execute_command(char *cmd_line, char **envp);
char	*get_path(char *cmd, char **envp);

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