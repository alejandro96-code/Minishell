/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 19:19:47 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/15 00:31:03 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!lst)
		return ;
	last = ft_lstlast(*lst);
	if (!last)
		*lst = new;
	else
		last->next = new;
}

/* #include <stdio.h>

int main() 
{
    t_list *node1;
	t_list *prueba;
	node1 = NULL;
	ft_lstadd_front(&node1, ft_lstnew("f"));
	ft_lstadd_back(&node1, ft_lstnew("f"));
	ft_lstadd_back(&node1, ft_lstnew("adios"));
	prueba = ft_lstlast(node1);
    printf("Último nodo con contenido: %s\n", (char *)prueba->content);
    // free(node1);
    return 0;
}  */