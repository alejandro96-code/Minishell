/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/05/11 13:34:37 by alejandro        ###   ########.fr       */
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

typedef struct s_command
{
	char	**argv;
	int		argc;
	int		is_builtin;
}			t_command;

// builtin del CD
char		*get_env_var(char *name, char **env);
int			builtin_cd(char **args, char **env);
char		*resolve_home_path(char *path, char **env);
char		*resolve_relative_path(char *path);
int			try_change_directory(char *path);

// builtin del echo, env, exit y pwd
int			builtin_echo(char **args, char **env);
int			builtin_env(char **env);
int			builtin_exit(char **args);
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
int			execute_builtin(char **args, char ***env);
void		execute_external(char **args, char **env);

// exec_cmd.c
void		ft_free_split(char **split);
char		*get_path(char *cmd, char **envp);
char		**extract_paths(char **envp);
char		*search_in_paths(char **paths, char *cmd);
void		execute_command(char *cmd_line, char **envp);

// parser.c
int			is_builtin_command(const char *cmd);
t_command	*parse_input(const char *input);

// funciones del main
char		**copy_env(char **envp);
char		*remove_quotes(char *str);
char		*clean_input(char *input);
char		*get_prompt(char **env);
void		process_input(char *input, char ***env);
int			main(int argc, char **argv, char **envp);

// Pipes
int			count_commands_and_split(char *input, char ***commands);
void		setup_pipes_and_fork(int i, int cmd_count, int pipefd[2],
				int *prev_pipe);
char		**parse_args_and_handle(char *command, char **env);
void		child_exec_or_builtin(char *command, char ***env);
int			execute_pipeline(char *input, char **env);

// Redirecciones
int			redirect_input(char *filename);
int			redirect_output(char *filename, int append);
int			heredoc(char *delimiter, char **env);
void		handle_redirections(char ***args, char **env);

// signals.c
void		sigint_handler(int sig);
void		sigquit_handler(int sig);
void		setup_signal_handlers(void);
void		setup_child_signals(void);
void		reset_signal_handlers(void);

// utils > expand_variable
char		*expand_variable(char *str, char **env);
int			process_env_variable(char *str, char *result, int *j, char **env);

// utils > find_user
char		*find_user(char **env);

// utils > autocomplete
void		free_command_names(void);
void		initialize_command_names(char **env);
char		*command_generator(const char *text, int state);
char		*file_generator(const char *text, int state);
char		**command_completion(const char *text, int start, int end);
void		setup_autocomplete(char **env);

// Wildcards
int			contains_wildcard(const char *str);
int			match_pattern(const char *pattern, const char *filename);
int			count_matching_files(const char *pattern);
char		**expand_wildcards(const char *arg, int *num_expanded);
char		**expand_wildcards_in_args(char **args, int *num_args);

#endif