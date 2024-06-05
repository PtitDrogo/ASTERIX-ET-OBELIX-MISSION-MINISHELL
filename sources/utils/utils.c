/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 14:47:08 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 15:10:41 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_char_in_str(char *str, char c)
{
	int	i;

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

char	*ft_strncat(char *src, char *dst, int len)
{
	int	i;
	int	size;

	i = 0;
	size = ft_len(dst);
	while (src[i] && i < len)
	{
		dst[size + i] = src[i];
		i++;
	}
	dst[size + i] = '\0';
	return (dst);
}

//count what nth node the node is
int	count_nodes(t_env *root)
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

t_env	*get_env_node(t_env *root, char *variable_name)
{
	if (variable_name == NULL || root == NULL)
		return (NULL);
	while (root != NULL)
	{
		if (ft_strcmp(root->variable_name, variable_name) == 0)
			return (root);
		root = root->next;
	}
	return (NULL);
}
