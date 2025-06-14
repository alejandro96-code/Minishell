/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/14 09:34:21 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <sys/ioctl.h>
# include <signal.h>
# include <ctype.h>
# include <dirent.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>
# include <sys/stat.h>    // Para stat() y S_ISDIR
# include <limits.h>      // Para PATH_MAX
# define RED "\033[31m"    // Rojo
# define YELLOW "\033[33m" // Amarillo
# define RESET "\033[0m"   // Reset

// Variable global para el manejo de señales
extern volatile sig_atomic_t	g_signal_received;

typedef enum e_operator_type
{
	OP_NONE,
	OP_PIPE
}	t_operator_type;

typedef struct s_redirect
{
    char				*file;
    int					type;
    struct s_redirect	*next;
}	t_redirect;

typedef struct s_command
{
    char				**argv;
    int					argc;
    int					is_builtin;
    t_redirect			*redirections;
    struct s_command	*next;
}	t_command;

// builtin del CD
char		*get_env_var(char *name, char **env);
int			builtin_cd(char **args, char **env);
char		*resolve_home_path(char *path, char **env);
char		*resolve_relative_path(char *path);
int			try_change_directory(char *path);

// builtin del echo, env, exit y pwd
int			builtin_echo(char **args, char **env);
int			builtin_env(char **env);
int			builtin_exit(char **args, int exit_status);
int			builtin_pwd(char **env);

// builtin del export
int			builtin_export(char **args, char ***env);
int			handle_export(char *arg, char ***env);
int			replace_env_value(char *key, char *new_value, char ***env);
int			add_env_value(char *new_value, char ***env);

// builtin del unset
int			builtin_unset(char **args, char ***env);
void		unset_variable(char *var, char ***env);
void		remove_env_entry(int index, char ***env);

// general_build.c
int			is_builtin(char *cmd);
int			execute_builtin(char **args, char ***env, int *exit_status);
int			execute_external(char **args, char **env);

// exec_cmd.c
void		ft_free_split(char **split);
char		*find_command_path(char *cmd, char **env);
void		exec_child_process(char **args, char **env, char *cmd_path);
int			exec_parent_process(pid_t pid);
char		**extract_paths(char **envp);
char		*search_in_paths(char **paths, char *cmd);
void		execute_command(char *cmd_line, char **envp);

// parser.c
t_command	*parse_command_input(const char *input, int *exit_status);
int			is_builtin_command(const char *cmd);

// Command parsing functions
t_command	*parse_command_input(const char *input, int *exit_status);
int			is_builtin_command(const char *cmd);

// Argument processing functions  
char		**split_command_args(char *str);
int			count_command_args(char *str);
char		*extract_next_argument(char **str);
void		skip_whitespace(char **str);

// Cleanup functions (centralized memory management)
void		ft_free_split(char **split);
void		free_string_array(char **array);
void		free_env(char **env);
char		**free_args(char **args);
void		free_command(t_command *cmd);
void		cleanup_and_exit(char **env);

// env_utils.c
void		free_env(char **env);

// input_processor.c
void		cleanup_and_exit(char **env);

// Pipes
int			count_commands_and_split(char *input, char ***commands);
void		setup_pipes_and_fork(int i, int cmd_count, int pipefd[2],
				int *prev_pipe);
char		**parse_args_and_handle(char *command, char **env);
void		child_exec_or_builtin(char *command, char ***env);
int			execute_pipeline(char *input, char **env);

// Pipe utils
int			split_and_validate_commands(char *input, char ***commands);
void		handle_child_process(int i, int cmd_count, int pipefd[2],
				int *prev_pipe);

// Pipe exec
char		**parse_command_arguments(char *command, char **env);
void		execute_child_command(char *command, char ***env);

// Pipe handler
void		process_pipeline_step(int i, int cmd_count, char **commands,
				char **env);
int			run_command_pipeline(char *input, char **env);

// Redirecciones
int			redirect_input(char *filename);
int			redirect_output(char *filename, int append);
int			heredoc(char *delimiter, char **env);
void		handle_redirections(char ***args, char **env);
int			check_redirections_exist(char **args);
char		**create_filtered_args(char **args, int count, char **env);

// signals.c
void		sigint_handler(int sig);
void		sigquit_handler(int sig);
void		setup_signal_handlers(void);
void		setup_child_signals(void);
void		reset_signal_handlers(void);

// utils > expand_variable
char		*expand_variable(char *str, char **env, int exit_status);
char		*process_quotes_and_variables(char *input, char **env, int exit_status);
char		*get_env_value(char *var_name, char **env);
int			process_env_variable(char *str, char *result, int *j, char **env);


// utils > find_user
char		*find_user(char **env);

// Error handling utility
void		write_error_msg(const char *msg);
void		write_error_parts(const char *part1, const char *part2, const char *part3);
void		write_command_not_found_error(const char *command);
void		write_export_error(const char *identifier);
void		write_exit_error(const char *arg);
void		write_variable_not_found_warning(const char *var_name);

// Parser args
char		**ft_split_args(char *str);
int			count_args(char *str);
char		*extract_next_arg(char **str);
void		skip_spaces(char **str);
char		**free_args(char **args);

// funciones del main
char		**copy_env(char **envp);
char		*remove_quotes(char *str);
char		*clean_input(char *input);
char		*get_prompt(char **env);
void		process_input(char *input, char ***env, int *exit_status);

#endif