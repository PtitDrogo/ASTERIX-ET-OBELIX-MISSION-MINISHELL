/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 12:48:32 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 16:19:59 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset(t_env *env, char *env_to_find)
{
	int	len_env;
	t_env *current;
	
	current = env;
	if (!env_to_find || !env) // uh ?
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
	return (0); // couldnt find env variable
}