/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 03:14:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:28:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		up_quote(char c, char *current_quotes)
{
	if (c == *current_quotes)
	{	
		*current_quotes = '\0';
		return (0);
	}
	else if (c == '\'')
	{
		if (*current_quotes == '\0')
		{	
			*current_quotes = '\'';
			return (0);
		}
		return (1);
	}
	else if (c == '\"')
	{
		if (*current_quotes == '\0')
		{	
			*current_quotes = '\"';
			return (0);
		}
		return (1);
	}
	return (1);
}

int	chars_to_expand(char *str)
{
	int	i;
	
	if (str[0] == '?')
		return (1);
	if (ft_isalpha(str[0]) == 0 && str[0] != '_')
		return (0);
	i = 1;
	while (str[i])
	{
		if (ft_isalnum(str[i]) == 0 && str[i] != '_')
			return (i);
		i++;
	}
	return (i);
}

char	*create_string_to_expand(char *str, t_gc **gc)
{
	int		i;
	int		size;
	char	*new_str;
	
	size = chars_to_expand(str);
	new_str = malloc_trash(size + 1, gc);
	new_str[size] = '\0';
	i = -1;
	while(++i < size)
		new_str[i] = str[i];
	return(new_str);
}

bool	can_expand(char *current_quotes)
{
	if (*current_quotes == '\'')
		return (false);
	else
		return (true);
}

void	var_up(int *var_1, int *var_2, int add_to_1, int add_to_2)
{
	*var_1 += add_to_1;
	*var_2 += add_to_2;
}
