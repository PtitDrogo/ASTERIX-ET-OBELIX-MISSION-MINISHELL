/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recycle_trash.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 22:03:05 by ptitdrogo         #+#    #+#             */
/*   Updated: 2024/06/05 16:19:59 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_node_env_node(t_gc	*node_to_pop, t_env	*env);
int		free_and_pop_gc(t_gc	**gc, t_gc	*node_to_pop);

void	recycle_trash_new(t_gc	**gc, t_env	*env)
{
	t_gc	*current_gc_node;
	t_gc	*save_point;

	current_gc_node = *gc;
	while (current_gc_node)
	{
		save_point = current_gc_node->next;
		if (is_node_env_node(current_gc_node, env) == false)
		{
			free_and_pop_gc(gc, current_gc_node);
		}
		current_gc_node = save_point;
	}
	return ;
}

bool	is_node_env_node(t_gc	*node_to_pop, t_env	*env)
{
	while (env)
	{
		if (node_to_pop->to_free == env)
			return (true);
		env = env->next;
	}
	return (false);
}

int	free_and_pop_gc(t_gc **gc, t_gc *node_to_pop)
{
	t_gc	*tmp;

	tmp = *gc;
	if (!gc || !node_to_pop || *gc)
		return (0);
	if (tmp == *gc)
	{
		*gc = (*gc)->next;
		free(node_to_pop->to_free);
		free(node_to_pop);
		return (1);
	}
	while (tmp->next && tmp->next != node_to_pop)
		tmp = tmp->next;
	if (tmp->next == NULL)
		return (0);
	tmp->next = tmp->next->next;
	free(node_to_pop->to_free);
	free(node_to_pop);
	return (1);
}
