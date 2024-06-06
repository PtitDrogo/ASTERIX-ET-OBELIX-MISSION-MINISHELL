/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 12:48:32 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 13:50:17 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset(t_env *env, char *env_to_find)
{
	t_env *current;
	
	current = env;
	if (!env_to_find || !env)
		return (0);
	while (current)
	{
		if (ft_strcmp(current->variable_name, env_to_find) == 0)
		{	
			pop(env, current);
			return (0);
		}
		current = current->next;
	}
	return (0);
}