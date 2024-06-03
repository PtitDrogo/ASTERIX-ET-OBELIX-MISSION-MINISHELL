/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 12:48:32 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:45:35 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset(t_env *env_dup_root, char *env_to_find)
{
	int	len_env;
	t_env *current;
	
	current = env_dup_root;
	if (!env_to_find || !env_dup_root) // uh ?
		return (0);
	while (current)
	{
		if (ft_strcmp(current->variable_name, env_to_find) == 0)
		{	
			pop(env_dup_root, current);
			return (0);
		}
		current = current->next;
	}
	return (0); // couldnt find env variable
}