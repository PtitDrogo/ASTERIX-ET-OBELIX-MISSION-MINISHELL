/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:54:23 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:45:35 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int env(t_env *env_dup_root, t_gc *gc)
{
    while (env_dup_root)
	{
		//we only print if the variable actually has a value
        if (env_dup_root->variable)
			if (printf("%s=%s\n", env_dup_root->variable_name, env_dup_root->variable) == -1)
                perror_exit(gc, errno, WRITE_ERR_MSG);
        env_dup_root = env_dup_root->next;
	}
    return (0);
}