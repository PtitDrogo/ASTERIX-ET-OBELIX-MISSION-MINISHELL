/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 14:47:08 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/09 17:16:45 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	len_to_char(char *str, char c)
{
	int	i;

	i = 0;
	while (str[i] != c && str[i])
		i++;
	return (i);
}


int	is_char_in_str(char *str, char c)
{
	int i;

	i = 0;
	if (str == NULL)
		return (0);
	while (str[i])
	{
		if (str[i] == c)
			return (1);
		i++;
	}
	return (0);
}

//count what nth node the node is
int	count_nodes(t_env_node *root)
{
	int	i;

	i = 0;
	while (root)
	{
		i++;
		root = root->next;
	}
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while ((s1[i] || s2[i]))
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

t_env_node *get_env_node(t_env_node *root, char *variable_name)
{
	if (variable_name == NULL || root == NULL)
		return (NULL);
	while (root != NULL)
	{	
		// printf("get_env_node is searching for a match to %s\n", variable_name);
		// printf("current testing vs %s\n", root->variable_name);
		if (ft_strcmp(root->variable_name, variable_name) == 0)
			return (root);
		root = root->next;
	}
	return (NULL); //couldnt find variable with that name
}
//gets the ->variable of a node
char	*get_env_variable(t_env_node *root, char *variable_name)
{
	t_env_node *home_node;
	
	home_node = get_env_node(root, variable_name);
	if (home_node == NULL)
		return (NULL);
	return (home_node->variable);
}

//returns true if cmd is a builtin
bool	is_builtin(char **cmd)
{
	static char *builtins[] = {"unset", "export", "env", "exit", "pwd", "cd", "echo", NULL};
	int	i;
	
	i = 0;
	if (cmd == NULL)
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
	return(i);
}