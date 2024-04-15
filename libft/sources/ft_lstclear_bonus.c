/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 14:13:00 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/22 18:28:47 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*temp;

	if (!lst || !(*del))
		return ;
	while (*lst)
	{
		temp = (*lst)->next;
		ft_lstdelone(*lst, (*del));
		*lst = temp;
	}
	*lst = NULL;
}
