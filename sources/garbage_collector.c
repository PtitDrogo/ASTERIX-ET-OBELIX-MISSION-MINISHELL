/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 17:32:57 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/22 20:28:50 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		no_dupplicate_check(void	*data, t_garbage_collect *gc);

//creates a new node to store it into the gc
int	add_to_trash(t_garbage_collect **root, void *to_free)
{
	t_garbage_collect	*new_node;
	t_garbage_collect	*current;

	new_node = malloc(sizeof(t_garbage_collect));
	if (!new_node)
		return (empty_trash(*root), 0); //write a message maybe ? TODO (this will need to free two gc later);
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
    if (add_to_trash(gc, to_return) == 0)
	{
		write(2, "malloc error\n", 13); //use proper stderror later;
		free(to_return);
		empty_trash_exit(*gc, MALLOC_ERROR);
	}
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

//this adds a malloced pointer to garbage collect
void	*setter_gc(void *data_to_set, t_garbage_collect **gc)
{
	if (no_dupplicate_check(data_to_set, *gc) == 1)
		add_to_trash(gc, data_to_set);
	return (data_to_set);
}

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

//so we dont add the same data twice
int		no_dupplicate_check(void	*data, t_garbage_collect *gc)
{
	while (gc)
	{
		if (gc->to_free == data)
			return (0);
		gc = gc->next;
	}
	return (1);
}

//TODO change this into gc
/*int	pop(t_env_node *env_dup_root, t_env_node *node_to_pop)
{	
	if (!env_dup_root || !node_to_pop)
		return (0); //gotta check later;
	while (env_dup_root->next && env_dup_root->next != node_to_pop)	
		env_dup_root = env_dup_root->next;
	if (env_dup_root->next == NULL)
		return (0); // we couldnt find the node to pop
	env_dup_root->next = env_dup_root->next->next;
	node_to_pop->variable = NULL;
	return (1);
}*/
