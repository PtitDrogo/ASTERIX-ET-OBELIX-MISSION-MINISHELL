/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 05:47:59 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:21:19 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*env_var(t_env_node *root, char *variable_name)
{
	t_env_node	*home_node;

	home_node = get_env_node(root, variable_name);
	if (home_node == NULL)
		return (NULL);
	return (home_node->variable);
}

bool	is_builtin(char **cmd)
{
	static char	*builtins[] = {"unset", "export", "env", "exit",
		"pwd", "cd", "echo", NULL};
	int			i;

	i = 0;
	if (cmd == NULL || cmd[0] == NULL)
		return (false);
	while (builtins[i])
	{
		if (ft_strcmp(cmd[0], builtins[i]) == 0)
			return (true);
		i++;
	}
	return (false);
}

int	count_arrays_in_doubleptr(void **array)
{
	int	i;

	i = 0;
	if (array == NULL)
		return (0);
	while (array[i])
	{
		i++;
	}
	return (i);
}
