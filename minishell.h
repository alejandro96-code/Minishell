/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/15 13:37:15 by alejandro        ###   ########.fr       */
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
// static void	output_char_sequence(const char *sequence, size_t len);
// static int	parse_echo_options(char *arg, int *newline, int *interpret_escapes)
// static void	print_basic_escapes(char c);
// static void	print_with_escapes(char *str)

// builtins/env.c
int			builtin_env(char **env);

// builtins/exit.c
int			builtin_exit(char **args, int exit_status);
// static int	is_numeric(char *str);

// builtins/export.c
int			builtin_export(char **args, char ***env);
int			handle_export(char *arg, char ***env);
int			replace_env_value(char *key, char *new_value, char ***env);
int			add_env_value(char *new_value, char ***env);
// static int	is_valid_export(char *str);

// builtins/general_build.c
int			is_builtin(char *cmd);
int			execute_builtin(char **args, char ***env, int *exit_status);

// builtins/pwd.c
int			builtin_pwd(char **env);

// builtins/unset.c
int			builtin_unset(char **args, char ***env);
void		unset_variable(char *var, char ***env);
void		remove_env_entry(int index, char ***env);
// static int	find_env_var_index(char **env, char *name);

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
// static char	*get_cmd_path_absolute(char *cmd);
// static char	*get_cmd_path_relative(char *cmd, char **env);
// static void	handle_fork_error(char *cmd_path);
// static int	execute_fork_process(char **args, char **env, char *cmd_path);

// executors/exec_utils.c
char		**extract_paths(char **envp);
char		*get_command_full_path(char *cmd, char **env);
// static int	validate_command_path(char *cmd_path);
// static char	*get_absolute_path(char *cmd);
// static char	*get_relative_path(char *cmd, char **env);

// executors/exec_path.c
char		*search_in_paths(char **paths, char *cmd);
// static char	*build_full_path(char *dir, char *cmd);
// static int	is_valid_command(char *path);
// static int	is_absolute_path(char *cmd);

// executors/exec_process.c
void		execute_command(char *cmd_line, char **envp);
// static void	handle_signal_interrupt(int sig);
// static int	handle_signaled_process(int status);
// static int	handle_exited_process(int status);

// executors/exec_utils.c (static)
// static int	validate_command_path(char *cmd_path);
// static char	*get_absolute_path(char *cmd);
// static char	*get_relative_path(char *cmd, char **env);

// parser/argument_splitter.c
char		**split_command_args(char *str);
int			count_command_args(char *str);
char		*extract_next_argument(char **str);
void		skip_whitespace(char **str);
// static void	skip_current_arg(char **str);

// parser/command_parser.c
t_command	*parse_command_input(const char *input, int *exit_status);
int			is_builtin_command(const char *cmd);
// static int	is_empty_or_whitespace(const char *str);
// static int	validate_quotes(const char *input);
// static t_command	*init_command(void);
// static int	setup_command_args(t_command *cmd, const char *input);

// pipes/pipe_exec.c
char		**parse_command_arguments(char *command, char **env);
void		execute_child_command(char *command, char ***env);
// static void	process_pipe_command_args(char **args, char **env);
// static void	execute_builtin_child(char **args, char ***env);
// static void	execute_external_child(char **args, char **env);

// pipes/pipe_handler.c
void		process_pipeline_step(int i, int cmd_count, char **commands,
				char **env);
int			run_command_pipeline(char *input, char **env);
// static void	setup_child_pipes(int i, int cmd_count, int pipefd[2], int *prev_pipe);
// static void	setup_parent_pipes(int i, int cmd_count, int pipefd[2], int *prev_pipe);
// static void	wait_for_children(int cmd_count);

// pipes/pipe_utils.c
int			split_and_validate_commands(char *input, char ***commands);
void		handle_child_process(int i, int cmd_count, int pipefd[2],
				int *prev_pipe);
// static int	check_pipe_syntax_error(char *input);
// static int	validate_commands(char ***commands);

// redirections/redirections.c
int			redirect_input(char *filename);
int			redirect_output(char *filename, int append);
// static int	count_string_args(char **args);

// redirections/redirections_basic.c
int			backup_file_descriptors(void);
void		restore_file_descriptors(int original_stdin, int original_stdout);

// redirections/redirections_handler.c
void		handle_redirections(char ***args, char **env);
int			check_redirections_exist(char **args);
char		**create_filtered_args(char **args, int count, char **env);
// static void	handle_redirection_type(char **args, int *i, char *cleaned_filename, char **env);
// static void	process_redirection(char **args, int *i, char **env);

// redirections/redirections_heredoc.c
int			heredoc(char *delimiter, char **env);
// static void	write_expanded_line(int write_fd, char *line, char **env);
// static void	read_heredoc_input(int write_fd, char *delimiter, char **env);

// signals/signals.c
void		sigint_handler(int sig);
void		sigquit_handler(int sig);
void		setup_signal_handlers(void);
void		reset_signal_to_default(void);
// static void	safe_write(int fd, const void *buf, size_t count);

// utils/env_utils.c
int			count_env_vars(char **env);
char		**copy_env_array(char **env);

// utils/expand_variable.c
char		*expand_variable(char *str, char **env, int exit_status);
char		*process_quotes_and_variables(char *input, char **env, int exit_status);
int			process_env_variable(char *str, char *result, int *j, char **env);
// static int	resize_result_buffer(t_expand_state *state);
// static int	add_char_to_result(t_expand_state *state, char c);
// static int	add_string_to_result(t_expand_state *state, const char *str);
// static int	extract_braced_var_name(t_expand_state *state, char *var_name);
// static int	extract_var_name(t_expand_state *state, char *var_name);
// static int	expand_variable_internal(t_expand_state *state);
// static int	process_escape_in_double_quotes(t_expand_state *state);

// utils/find_user.c
char		*find_user(char **env);

// utils/input_processor.c
char		*get_prompt(char **env);
void		process_input(char *input, char ***env, int *exit_status);
void		cleanup_and_exit(char **env);
// static void	expand_command_args(t_command *cmd, char **env, int exit_status);
// static void	process_single_command(t_command *cmd, char ***env, int *exit_status);

// utils/prompt_utils.c
char		*get_current_directory_color(void);
char		*format_prompt_with_colors(char *user, char *dir);
char		*build_complete_prompt(char *user, char *dir);
// static char	*build_prompt_part1(char *username);
// static char	*build_prompt_part2(char *prompt, char *cwd);

// utils/quote_utils.c
char		*remove_quotes(char *str);

#endif