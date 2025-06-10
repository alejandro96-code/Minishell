/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 19:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/10 19:00:00 by alejandro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	write_error_msg(const char *msg)
{
	if (write(STDERR_FILENO, msg, ft_strlen(msg)) == -1)
	{
		/* Silent fail - error writing to stderr */
	}
}

void	write_error_parts(const char *part1, const char *part2, const char *part3)
{
	write_error_msg(part1);
	if (part2)
		write_error_msg(part2);
	if (part3)
		write_error_msg(part3);
}

void	write_command_not_found_error(const char *command)
{
	write_error_parts(command, ": command not found\n", NULL);
}

void	write_export_error(const char *identifier)
{
	write_error_parts("export: `", identifier, "': not a valid identifier\n");
}

void	write_exit_error(const char *arg)
{
	write_error_parts("exit: ", arg, ": numeric argument required\n");
}

void	write_variable_not_found_warning(const char *var_name)
{
	write_error_parts("Warning: Variable ", var_name, " not found in environment\n");
}
