/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 18:57:09 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 15:16:50 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	ft_is_name_lower(t_env *new_str, t_env *low_str);
static int	export_print(t_env *node_to_print, t_gc *gc);
static bool	get_return_value(char new_str, char low_str);

int	sorted_env_print(t_env *env, t_gc *gc)
{
	t_env	*lowest_name;
	t_env	*current;
	t_env	*last_node_printed;
	int		n_nodes;

	n_nodes = count_nodes(env);
	last_node_printed = NULL;
	while (n_nodes)
	{
		current = env;
		lowest_name = NULL;
		while (current)
		{
			if (ft_is_name_lower(current, lowest_name)
				&& ft_is_name_lower(last_node_printed, current))
			{
				lowest_name = current;
			}
			current = current->next;
		}
		export_print(lowest_name, gc);
		n_nodes--;
		last_node_printed = lowest_name;
	}
	return (0);
}

static int	export_print(t_env *node_to_print, t_gc *gc)
{
	if (node_to_print == NULL)
		return (0);
	if (printf("declare -x %s", node_to_print->variable_name) == -1)
		perror_exit(gc, errno, WRITE_ERR_MSG);
	if (node_to_print->variable)
	{
		if (printf("=\"%s\"", node_to_print->variable) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	if (printf("\n") == -1)
		perror_exit(gc, errno, WRITE_ERR_MSG);
	node_to_print = node_to_print->next;
	return (1);
}

static bool	ft_is_name_lower(t_env *new_node, t_env *low_node)
{
	int		i;
	char	*low_str;
	char	*new_str;

	i = 0;
	if (low_node == NULL || new_node == NULL)
		return (true);
	if (new_node == low_node)
		return (false);
	low_str = low_node->variable_name;
	new_str = new_node->variable_name;
	if (low_str == NULL)
		return (true);
	if (new_str == NULL)
		return (false);
	while (low_str[i] && new_str[i])
	{
		if (new_str[i] < low_str[i])
			return (true);
		else if (new_str[i] > low_str[i])
			return (false);
		i++;
	}
	return (get_return_value(new_str[i], low_str[i]));
}

static bool	get_return_value(char new_str, char low_str)
{
	if (new_str == '\0' && low_str)
		return (true);
	return (false);
}
