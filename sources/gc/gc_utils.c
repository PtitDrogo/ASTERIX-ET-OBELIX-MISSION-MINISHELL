/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 05:54:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 06:07:44 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	**setter_double_p_gc(void **data_to_set, t_garbage_collect **gc)
{
	int	i;

	i = 0;
	while (data_to_set[i])
	{
		setter_gc(data_to_set[i], gc);
		i++;
	}
	setter_gc(data_to_set, gc);
	return (data_to_set);
}

void	malloc_check(void *ptr, t_garbage_collect *gc)
{
	if (ptr == NULL)
	{
		if (ft_printf2(MALLOC_ERR_MSG) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(gc, MALLOC_ERROR);
	}
	return ;
}
