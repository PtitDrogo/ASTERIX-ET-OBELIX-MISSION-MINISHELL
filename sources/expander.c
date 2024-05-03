/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/03 13:32:54 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//I coded this assuming Ill get the full thing with the dollar sign (exemple = $MIAOU)
//return the expanded string
char    *expander(t_env_node *env, t_garbage_collect **gc, char *to_expand)
{
	char    *str_to_return;
	
	to_expand = &to_expand[1]; //small ass line to go past the $ 
	if (!env || !to_expand)
		return (NULL);
	while (env) 
	{
		if (env->variable_name && ft_strcmp(env->variable_name, to_expand) == 0)
		{
			if (env->variable == NULL)
				return (NULL);
			str_to_return = setter_gc(ft_strdup(env->variable), gc);
			malloc_check(str_to_return, *gc);
			return (str_to_return);
		}
		env = env->next;
	}
	return (NULL);
}
