/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 17:16:04 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/04/07 17:17:34 by dgasco-g         ###   ########.fr       */
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
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"
# define red = "\033[31m";    // Rojo
# define yellow = "\033[33m"; // Amarillo
# define reset = "\033[0m";  


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

// funciones de builtin
int builtin_echo(char **args, char **env);
int builtin_cd(char **args, char **env);
int builtin_pwd(char **env);
int builtin_export(char **args, char ***env);
int builtin_unset(char **args, char ***env);
int builtin_env(char **env);
int builtin_exit(char **args);

//utils
char	*expand_variable(char *str, char **env);
char	*find_user(char **env);

#endif