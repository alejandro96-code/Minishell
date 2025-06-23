/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alejanr2 <alejanr2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 14:30:00 by alejandro         #+#    #+#             */
/*   Updated: 2025/06/23 14:48:56 by alejanr2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Función wrapper para eliminación de comillas
// Utilizada en múltiples módulos para mantener compatibilidad de interfaz
char	*remove_quotes(char *str)
{
	if (!str)
		return (NULL);
	return (str);
}

// Actualiza el estado de las comillas durante el parsing
void	update_quote_state(char c, int *in_single_quotes, int *in_double_quotes)
{
	if (c == '\'' && !*in_double_quotes)
		*in_single_quotes = !*in_single_quotes;
	else if (c == '"' && !*in_single_quotes)
		*in_double_quotes = !*in_double_quotes;
}
