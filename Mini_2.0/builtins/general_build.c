/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general_build.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 23:56:33 by dgasco-g          #+#    #+#             */
/*   Updated: 2025/06/05 19:35:19 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_builtin(char *cmd)
{
	return (!ft_strncmp(cmd, "cd", 2) && cmd[2] == '\0') 
		|| (!ft_strncmp(cmd, "echo", 4) && cmd[4] == '\0') 
		|| (!ft_strncmp(cmd, "pwd", 3) && cmd[3] == '\0')
		|| (!ft_strncmp(cmd, "export", 6) && cmd[6] == '\0') 
		|| (!ft_strncmp(cmd, "unset", 5) && cmd[5] == '\0') 
		|| (!ft_strncmp(cmd, "env", 3) && cmd[3] == '\0')
		|| (!ft_strncmp(cmd, "exit", 4) && cmd[4] == '\0');
}

int	execute_builtin(char **args, char ***env)
{
	if (!ft_strncmp(args[0], "cd", 2) && args[0][2] == '\0')
		return (builtin_cd(args, *env));
	if (!ft_strncmp(args[0], "echo", 4) && args[0][4] == '\0')
		return (builtin_echo(args, *env));
	if (!ft_strncmp(args[0], "pwd", 3) && args[0][3] == '\0')
		return (builtin_pwd(*env));
	if (!ft_strncmp(args[0], "export", 6) && args[0][6] == '\0')
		return (builtin_export(args, env));
	if (!ft_strncmp(args[0], "unset", 5) && args[0][5] == '\0')
		return (builtin_unset(args, env));
	if (!ft_strncmp(args[0], "env", 3) && args[0][3] == '\0')
		return (builtin_env(*env));
	if (!ft_strncmp(args[0], "exit", 4) && args[0][4] == '\0')
		return (builtin_exit(args));
	return (1);
}
