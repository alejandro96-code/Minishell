/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/18 18:47:57 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <signal.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <unistd.h>
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
}	t_command;

typedef struct s_expand_state
{
	char	*input;
	char	*result;
	int		input_pos;
	int		result_pos;
	int		result_size;
	int		in_single_quotes;
	int		in_double_quotes;
	char	**env;
	int		exit_status;
}	t_expand_state;

// builtins/cd.c
char		*get_env_var(char *name, char **env);
int			builtin_cd(char **args, char **env);
char		*resolve_home_path(char *path, char **env);
char		*resolve_relative_path(char *path);
int			try_change_directory(char *path);

// builtins/echo.c
int			builtin_echo(char **args, char **env);

// builtins/env.c
int			builtin_env(char **env);

// builtins/exit.c
int			builtin_exit(char **args, int exit_status);

// builtins/export.c
int			builtin_export(char **args, char ***env);
int			handle_export(char *arg, char ***env);
int			replace_env_value(char *key, char *new_value, char ***env);
int			add_env_value(char *new_value, char ***env);

// builtins/general_build.c
int			is_builtin(char *cmd);
int			execute_builtin(char **args, char ***env, int *exit_status);

// builtins/pwd.c
int			builtin_pwd(char **env);

// builtins/unset.c
int			builtin_unset(char **args, char ***env);
void		unset_variable(char *var, char ***env);
void		remove_env_entry(int index, char ***env);

// cleanup/cleanup.c
void		ft_free_split(char **split);
void		free_env(char **env);
void		free_string_array(char **array);
char		**free_args(char **args);
void		free_command(t_command *cmd);

// executors/exec_cmd.c
char		*find_command_path(char *cmd, char **env);
void		exec_child_process(char **args, char **env, char *cmd_path);
int			exec_parent_process(pid_t pid);
int			execute_external(char **args, char **env);

// executors/exec_utils.c
char		**extract_paths(char **envp);
char		*get_command_full_path(char *cmd, char **env);

// executors/exec_path.c
char		*search_in_paths(char **paths, char *cmd);

// executors/exec_process.c
void		execute_command(char *cmd_line, char **envp);

// parser/argument_splitter.c
char		**split_command_args(char *str);
int			count_command_args(char *str);
char		*extract_next_argument(char **str);
void		skip_whitespace(char **str);

// parser/command_parser.c
t_command	*parse_command_input(const char *input, int *exit_status);
int			is_builtin_command(const char *cmd);
int			is_redirection(char c);
char		*aux_split_redirection(char **str, char *start);
int			validate_redirections(const char *input);

// pipes/pipe_exec.c
char		**parse_command_arguments(char *command, char **env);
void		execute_child_command(char *command, char ***env);

// pipes/pipe_handler.c
void		process_pipeline_step(int i, int cmd_count, char **commands,
				char **env);
int			run_command_pipeline(char *input, char **env);

// pipes/pipe_utils.c
int			split_and_validate_commands(char *input, char ***commands);
void		handle_child_process(int i, int cmd_count, int pipefd[2],
				int *prev_pipe);

// redirections/redirections.c
int			redirect_input(char *filename);
int			redirect_output(char *filename, int append);
// redirections/redirections_basic.c
int			backup_file_descriptors(void);
void		restore_file_descriptors(int original_stdin, int original_stdout);

// redirections/redirections_handler.c
void		handle_redirections(char ***args, char **env);
int			check_redirections_exist(char **args);
char		**create_filtered_args(char **args, int count, char **env);

// redirections/redirections_heredoc.c
int			heredoc(char *delimiter, char **env);

// signals/signals.c
void		sigint_handler(int sig);
void		sigquit_handler(int sig);
void		setup_signal_handlers(void);
void		reset_signal_to_default(void);

// utils/env_utils.c
int			count_env_vars(char **env);
char		**copy_env_array(char **env);

// utils/expand_variable.c
char		*expand_variable(char *str, char **env, int exit_status);
char		*process_quotes_and_vars(char *input, char **env, int exit_status);
int			process_env_variable(char *str, char *result, int *j, char **env);

// utils/find_user.c
char		*find_user(char **env);

// utils/input_processor.c
char		*get_prompt(char **env);
void		process_input(char *input, char ***env, int *exit_status);
void		cleanup_and_exit(char **env);

// utils/prompt_utils.c
char		*get_current_directory_color(void);
char		*format_prompt_with_colors(char *user, char *dir);
char		*build_complete_prompt(char *user, char *dir);

// utils/quote_utils.c
char		*remove_quotes(char *str);

#endif