/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 12:48:32 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/15 18:45:05 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	unset(t_env_node *env_dup_root, char *env_to_find)
{
	int	len_env;
	t_env_node *current;
	
	current = env_dup_root;
	if (!env_to_find || !env_dup_root) // uh ?
	{
		perror("Environnement does not exist");
		exit(EXIT_FAILURE);
	}
	len_env = ft_strlen(env_to_find) + 1;
	printf("IN UNSET : %s\n", env_to_find);
	while (current)
	{
		if (ft_strncmp(current->variable_name, env_to_find, len_env) == 0)
		{	
			printf("\n\nI found the variable\n\n");
			pop(env_dup_root, current);
			return (1); // here we return the whole string;
		}
		current = current->next;
	}
	return (0); // couldnt find env variable
}