/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:02:57 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 16:14:11 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	generate_env_llist(t_env **env, t_gc **gc, char **envp)
{
	int	i;

	i = -1;
	if (envp == NULL)
		return (0);
	while (envp[++i])
	{
		if (export(env, (void *)envp[i], gc) == 1)
			return (0);
	}
	return (1);
}

int	pop(t_env *env, t_env *node_to_pop)
{
	if (!env || !node_to_pop)
		return (0);
	while (env->next && env->next != node_to_pop)
		env = env->next;
	if (env->next == NULL)
		return (0);
	env->next = env->next->next;
	node_to_pop->variable = NULL;
	return (1);
}

int	is_valid_env_name(char *name, t_gc *gc)
{
	int	i;

	if (ft_isalpha(name[0]) == 0 && name[0] != '_')
	{
		if (ft_printf2("bash: export: `%s': \
			not a valid identifier\n", name) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (0);
	}
	i = 1;
	while (name[i] && name[i] != '=')
	{
		if (ft_isalnum(name[i]) == 0 && name[i] != '_')
		{
			if (ft_printf2("bash: export: `%s':\
				 not a valid identifier\n", name) == -1)
				perror_exit(gc, errno, WRITE_ERR_MSG);
			return (0);
		}
		i++;
	}
	return (1);
}
