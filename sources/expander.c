/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/24 18:03:04 by garivo           ###   ########.fr       */
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
			str_to_return = ft_strdup(env->variable);
			if (!str_to_return)
			{    
				if (ft_printf_err("Malloc failed\n") == -1)
					perror_exit(*gc, errno, "Failed to write error message");
				empty_trash_exit(*gc, MALLOC_ERROR);
			}
			setter_gc(str_to_return, gc);
			return (str_to_return);
		}
		env = env->next;
	}
	return (NULL);
}
