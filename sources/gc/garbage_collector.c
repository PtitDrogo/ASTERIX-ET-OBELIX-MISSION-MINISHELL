/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 17:32:57 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 15:21:03 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		no_dupplicate_check(void	*data, t_gc *gc);

int	add_to_trash(t_gc **root, void *to_free)
{
	t_gc	*new_node;
	t_gc	*current;

	new_node = malloc(sizeof(t_gc));
	if (!new_node)
		return (0);
	new_node->next = NULL;
	new_node->to_free = to_free;
	if ((*root) == NULL)
	{
		*root = new_node;
		return (1);
	}
	current = *root;
	while (current->next)
		current = current->next;
	current->next = new_node;
	return (1);
}

void	*malloc_trash(int size, t_gc **gc)
{
	void	*to_return;

	to_return = malloc(size);
	if (!to_return)
	{
		if (ft_printf2("Malloc failed\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(*gc, MALLOC_ERROR);
	}
	if (add_to_trash(gc, to_return) == 0)
	{
		if (ft_printf2("GC Malloc failed\n") == -1)
		{
			free (to_return);
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		}
		free (to_return);
		empty_trash_exit(*gc, MALLOC_ERROR);
	}
	return (to_return);
}

int	empty_trash(t_gc *gc)
{
	t_gc	*tmp_to_free;

	while (gc)
	{
		free (gc->to_free);
		tmp_to_free = gc;
		gc = gc->next;
		free (tmp_to_free);
	}
	return (1);
}

int	no_dupplicate_check(void	*data, t_gc *gc)
{
	while (gc)
	{
		if (gc->to_free == data)
			return (0);
		gc = gc->next;
	}
	return (1);
}

void	*setter_gc(void *data_to_set, t_gc **gc)
{
	if (data_to_set == NULL)
		return (data_to_set);
	if (no_dupplicate_check(data_to_set, *gc) == 1)
	{
		if (add_to_trash(gc, data_to_set) == 0)
		{
			free(data_to_set);
			if (ft_printf2(MALLOC_ERR_MSG) == -1)
				perror_exit(*gc, EXIT_FAILURE, WRITE_ERR_MSG);
			empty_trash_exit(*gc, MALLOC_ERROR);
		}
	}
	return (data_to_set);
}
