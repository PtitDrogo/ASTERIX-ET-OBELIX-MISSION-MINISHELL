/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 12:48:32 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/07 15:44:16 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset(t_env_node *env_dup_root, char *env_to_find)
{
	int	len_env;
	t_env_node *current;
	
	current = env_dup_root;
	if (!env_to_find || !env_dup_root) // uh ?
		return (0);
	while (current)
	{
		if (ft_strcmp(current->variable_name, env_to_find) == 0)
		{	
			pop(env_dup_root, current);
			return (1);
		}
		current = current->next;
	}
	return (0); // couldnt find env variable
}