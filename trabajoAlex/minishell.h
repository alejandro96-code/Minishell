/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 17:16:04 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/13 12:37:41 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <ctype.h>
# include <fcntl.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"
# define RED "\033[31m"    // Rojo
# define YELLOW "\033[33m" // Amarillo
# define RESET "\033[0m"   // Reset


typedef struct s_command {
    char **argv;     // Array de tokens (argumentos)
    int argc;        // Número de argumentos
    int is_builtin;  // Indica si es un comando builtin (1 si lo es, 0 si no)
} t_command;

// builtins
int builtin_cd(char **args, char **env);
int builtin_echo(char **args, char **env);
int builtin_env(char **env);
int builtin_exit(char **args);
int builtin_export(char **args, char ***env);
int builtin_pwd(char **env);
int builtin_unset(char **args, char ***env);

//general_build.c
int is_builtin(char *cmd);
int execute_builtin(char **args, char ***env);
void execute_external(char **args, char **env);

// exec_cmd.c
void	ft_free_split(char **split);
char	*get_path(char *cmd, char **envp);
void	execute_command(char *cmd_line, char **envp);

//parser.c
int is_builtin_command(const char *cmd);
t_command *parse_input(const char *input);

//funciones del main
char **copy_env(char **envp);
char *remove_quotes(char *str);
char *clean_input(char *input);
char	*get_prompt(char ** env);
void process_input(char *input, char ***env);
int main(int argc, char **argv, char **envp);

// Pipes
int count_commands_and_split(char *input, char ***commands);
void setup_pipes_and_fork(int i, int cmd_count, int pipefd[2], int *prev_pipe);
char **parse_args_and_handle(char *command, char **env);
void child_exec_or_builtin(char *command, char ***env);
int execute_pipeline(char *input, char **env);

// Redirecciones
int redirect_input(char *filename);
int redirect_output(char *filename, int append);
int heredoc(char *delimiter, char **env);
void handle_redirections(char ***args, char **env);

//utils > expand_variable
char *expand_variable(char *str, char **env, int last_exit_status);
int process_env_variable(char *str, char *result, int *j, char **env);

//utils > find_user
char	*find_user(char **env);

//utils > memory_utils
void safe_free(void **ptr);
void free_string_array(char ***array);
void free_command(t_command **cmd);
void *safe_malloc(size_t size);
char *safe_strdup(const char *str);
char **safe_copy_env(char **envp);

// Wildcards
int contains_wildcard(const char *str);
int match_pattern(const char *pattern, const char *filename);
int count_matching_files(const char *pattern);
char **expand_wildcards(const char *arg, int *num_expanded);
char **expand_wildcards_in_args(char **args, int *num_args);

#endif