/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 17:32:57 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/15 19:00:42 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	add_to_trash(t_garbage_collect **root, void *to_free)
{
	t_garbage_collect	*new_node;
	t_garbage_collect	*current;

	new_node = malloc(sizeof(t_garbage_collect));
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

void    *malloc_trash(int size, t_garbage_collect **gc)
{
    void *to_return;

    to_return = malloc(size);
    add_to_trash(gc, to_return);
    return(to_return);
}

int empty_trash(t_garbage_collect *gc)
{
	t_garbage_collect *tmp_to_free;
	
    while (gc)
	{
        free(gc->to_free);
		tmp_to_free = gc;
		gc = gc->next;
		free(tmp_to_free);
	}
	return (1);
}

void	*setter_gc(void *data_to_set, t_garbage_collect **gc)
{
	add_to_trash(gc, data_to_set);
	return (data_to_set);
}
