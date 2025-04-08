/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgasco-g <dgasco-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 15:29:51 by dgasco-g          #+#    #+#             */
/*   Updated: 2024/12/14 23:19:05 by dgasco-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	int	i;

	if (!lst)
		return (NULL);
	i = ft_lstsize(lst);
	while (--i)
		lst = lst->next;
	return (lst);
}

// #include <stdio.h>
// int main() {
//     t_list *node1;
// 	t_list *new;
// 	t_list *prueba;
// 	node1 = NULL;
// 	new = malloc(sizeof(t_list));
// 	new ->content = "hola";
// 	new ->next = NULL;
// 	ft_lstadd_front(&node1, ft_lstnew("f"));
// 	ft_lstadd_front(&node1, ft_lstnew("f"));
// 	ft_lstadd_front(&node1, ft_lstnew("adios"));
// 	ft_lstadd_front(&node1, new);
// 	prueba = ft_lstlast(node1);
//     printf("Último nodo con contenido: %s\n", (char *)prueba->content);
//     // free(node1);
//     return 0;
// }