/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 05:54:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 17:28:47 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_array_from_index_to_end(void **array, int i);

void	**setter_double_p_gc(void **data_to_set, t_gc **gc)
{
	int	i;

	i = 0;
	if (data_to_set == NULL)
		return (data_to_set);
	while (data_to_set[i])
	{
		if (no_dupplicate_check(data_to_set[i], *gc) == 1)
		{
			if ((add_to_trash(gc, data_to_set[i]) == 0))
			{
				free_array_from_index_to_end(data_to_set, i);
				if (ft_printf2(MALLOC_ERR_MSG) == -1)
					perror_exit(*gc, EXIT_FAILURE, WRITE_ERR_MSG);
				empty_trash_exit(*gc, MALLOC_ERROR);
			}
		}
		i++;
	}
	setter_gc(data_to_set, gc);
	return (data_to_set);
}

void	malloc_check(void *ptr, t_gc *gc)
{
	if (ptr == NULL)
	{
		if (ft_printf2(MALLOC_ERR_MSG) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(gc, MALLOC_ERROR);
	}
	return ;
}

void	free_array_from_index_to_end(void **array, int i)
{
	if (!array)
		return ;
	while (array[i])
	{
		if (array[i])
			free(array[i]);
		i++;
	}
	free(array);
	return ;
}
