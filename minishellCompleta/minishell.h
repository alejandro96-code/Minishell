/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 17:16:04 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/11 18:25:49 by dgasco-g         ###   ########.fr       */
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
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include "libft/libft.h"
# define red = "\033[31m";    // Rojo
# define yellow = "\033[33m"; // Amarillo
# define reset = "\033[0m";  


typedef struct s_command {
    char **argv;     // Array de tokens (argumentos)
    int argc;        // Número de argumentos
    int is_builtin;  // Indica si es un comando builtin (1 si lo es, 0 si no)
} t_command;

//funciones del main
char **copy_env(char **envp);
char *remove_quotes(char *str);
char *clean_input(char *input);
char	*get_prompt(char ** env);
void process_input(char *input, char ***env);
int main(int argc, char **argv, char **envp);

// exec_cmd.c
void	ft_free_split(char **split);
char	*get_path(char *cmd, char **envp);
void	execute_command(char *cmd_line, char **envp);

//parser.c
int is_builtin_command(const char *cmd);
t_command *parse_input(const char *input);

// buildins
int builtin_echo(char **args, char **env);
int builtin_cd(char **args, char **env);
int builtin_pwd(char **env);
int builtin_export(char **args, char ***env);
int builtin_unset(char **args, char ***env);
int builtin_env(char **env);
int builtin_exit(char **args);

//expand_variable
char *expand_variable(char *str, char **env, int last_exit_status);
int process_env_variable(char *str, char *result, int *j, char **env);

//cfind_user
char	*find_user(char **env);

// Redirecciones
int redirect_input(char *filename);
int redirect_output(char *filename, int append);
int heredoc(char *delimiter);
void handle_redirections(char ***args);

// Pipes
int execute_pipeline(char *input, char **env);
int count_commands_and_split(char *input, char ***commands);
void setup_pipes_and_fork(int i, int cmd_count, int pipefd[2], int *prev_pipe);
char **parse_args_and_handle(char *command);
void child_exec_or_builtin(char *command, char ***env);

// Señales
extern int g_signal_received;
void signal_handler(int signum);
void setup_signals(void);

//general
int is_builtin(char *cmd);
int execute_builtin(char **args, char ***env);
void execute_external(char **args, char **env);




#endif