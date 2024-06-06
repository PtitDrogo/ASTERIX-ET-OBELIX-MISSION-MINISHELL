/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 16:11:59 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 16:01:18 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env		*check_if_variable_exist(t_env *root, void *variable);
static char			*get_env_name(const char *src, t_gc **gc);
static char			*get_env_var(const char *src, t_gc **gc);
static int			set_same(t_env	*same_name_node, void *variable, t_gc **gc);

int	export(t_env **root, void *variable, t_gc **gc)
{
	t_env	*new_node;
	t_env	*current;
	t_env	*same_name_node;

	if (is_valid_env_name(variable, *gc) == 0)
		return (1);
	same_name_node = check_if_variable_exist(*root, variable);
	if (same_name_node)
		return (set_same(same_name_node, variable, gc));
	new_node = malloc_trash(sizeof(t_env), gc);
	new_node->next = NULL;
	new_node->variable_name = get_env_name(variable, gc);
	new_node->variable = get_env_var(variable, gc);
	if ((*root) == NULL)
	{
		*root = new_node;
		return (0);
	}
	current = *root;
	while (current->next)
		current = current->next;
	current->next = new_node;
	return (0);
}

static t_env	*check_if_variable_exist(t_env *root, void *variable)
{
	size_t	var_len;

	if (variable == NULL)
		return (NULL);
	if (is_char_in_str(variable, '=') == 0)
		return (NULL);
	var_len = len_to_char(variable, '=');
	while (root)
	{
		if (len_to_char(root->variable_name, '=') == var_len)
		{
			if (ft_strncmp(root->variable_name, variable, var_len) == 0)
				return (root);
		}
		root = root->next;
	}
	return (NULL);
}

static char	*get_env_name(const char *src, t_gc **gc)
{
	int		i;
	int		j;
	char	*dest;

	i = 0;
	while (src[i] != '\0' && src[i] != '=')
		i++;
	dest = malloc_trash(sizeof(char) * i + 1, gc);
	if (!dest)
		return (NULL);
	dest[i] = '\0';
	j = i;
	i = 0;
	while (i < j)
	{
		dest[i] = src[i];
		i++;
	}
	return (dest);
}

static char	*get_env_var(const char *src, t_gc **gc)
{
	int		i;
	char	*to_return;

	i = 0;
	while (src[i] != '\0' && src[i] != '=')
		i++;
	if (src[i] == '\0')
		return (NULL);
	i++;
	while (src[i] && ((src[i] >= 9 && src[i] <= 13) || src[i] == ' '))
		i++;
	to_return = setter_gc(ft_strdup(&src[i]), gc);
	malloc_check(to_return, *gc);
	return (to_return);
}

int	set_same(t_env	*same_name_node, void *variable, t_gc **gc)
{
	same_name_node->variable_name = get_env_name(variable, gc);
	same_name_node->variable = get_env_var(variable, gc);
	return (0);
}
