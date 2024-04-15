/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:54:23 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/15 14:57:14 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int env(t_env_node *env_dup_root)
{
    while (env_dup_root)
	{
		if (env_dup_root->variable)
			if (printf("%s=%s\n", env_dup_root->variable_name, env_dup_root->variable) == -1)
                return (0);
        env_dup_root = env_dup_root->next;
	}
    return (1);
}