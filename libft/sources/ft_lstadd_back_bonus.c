/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 13:18:03 by tfreydie          #+#    #+#             */
/*   Updated: 2024/02/10 10:19:48 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*previous_last;

	if (!new)
		return ;
	if (lst)
	{
		if (!*lst)
			*lst = new;
		else
		{
			previous_last = ft_lstlast(*lst);
			previous_last->next = new;
		}
	}
}
