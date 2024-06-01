/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recycle_trash.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 22:03:05 by ptitdrogo         #+#    #+#             */
/*   Updated: 2024/06/01 01:11:16 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_node_env_node(t_garbage_collect *node_to_pop, t_env_node    *env_dup_root);
int		free_and_pop_gc(t_garbage_collect **gc, t_garbage_collect *node_to_pop);

void	recycle_trash_new(t_garbage_collect **gc, t_env_node    *env_dup_root)
{
	t_garbage_collect *current_gc_node;
	t_garbage_collect *save_point;
	
	current_gc_node = *gc;
	while (current_gc_node)
	{
		save_point = current_gc_node->next;
		if (is_node_env_node(current_gc_node, env_dup_root) == false)
		{	
			free_and_pop_gc(gc, current_gc_node);
		}
		current_gc_node = save_point;
	}
	return ;
}
bool	is_node_env_node(t_garbage_collect *node_to_pop, t_env_node    *env_dup_root)
{
	while (env_dup_root)
	{
		if (node_to_pop->to_free == env_dup_root)
			return(true);
		env_dup_root = env_dup_root->next;
	}
	return (false);
}

int	free_and_pop_gc(t_garbage_collect **gc, t_garbage_collect *node_to_pop)
{	
	t_garbage_collect *tmp;

	tmp = *gc;
	if (!gc || !node_to_pop || *gc)
		return (0); //gotta check later;

	if (tmp == *gc) 
	{
    	*gc = (*gc)->next;
        free(node_to_pop->to_free);
        free(node_to_pop);
        return 1;
    }
	while (tmp->next && tmp->next != node_to_pop)	
		tmp = tmp->next;
	if (tmp->next == NULL)
		return (0); // we couldnt find the node to pop
	tmp->next = tmp->next->next;
	free(node_to_pop->to_free);
	free(node_to_pop);
	return (1);
}