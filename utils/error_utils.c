/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejandro <alejandro@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 19:00:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/14 13:36:33 by alejandro        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	write_error_parts(const char *part1, const char *part2,
		const char *part3)
{
	ft_putstr_fd((char *)part1, STDERR_FILENO);
	if (part2)
		ft_putstr_fd((char *)part2, STDERR_FILENO);
	if (part3)
		ft_putstr_fd((char *)part3, STDERR_FILENO);
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
