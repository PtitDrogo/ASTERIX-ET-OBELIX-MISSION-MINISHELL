/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:54:23 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 13:01:44 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_builtin(t_env *env, t_gc *gc)
{
	while (env)
	{
		if (env->variable)
			if (printf("%s=%s\n", env->variable_name, env->variable) == -1)
				perror_exit(gc, errno, WRITE_ERR_MSG);
		env = env->next;
	}
	return (0);
}
