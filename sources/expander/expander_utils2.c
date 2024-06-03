/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 06:02:54 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:23:45 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_question_mark(t_expand *expdr, int *i, char **tmp)
{
	(*i) += ft_len(*tmp) + 1;
	if (*tmp && (*tmp)[0] == '?')
	{	
		*tmp = expdr->error_value;
		return (1);
	}
	return (0);
}
int	tmp_check(t_expand *expdr, int *i, char *tmp)
{
	if (ft_len(tmp) == 0)
	{	
		expdr->expanded_var[expdr->size++] = '$';
		(*i)++;
		return (1);
	}
	return (0);
}

int	check_quotes(t_expand *expdr, int *i)
{
	if (expdr->array[*i] == '\'' || expdr->array[*i] == '\"')
	{	
		if (up_quote(expdr->array[*i], &expdr->current_quotes) == 1 || expdr->mode == EXPAND)
			expdr->expanded_var[expdr->size++] = expdr->array[(*i)++];
		else
			(*i)++;
		return (1);
	}
	return (0);
}


void	fill_string(t_expand *expdr, char *tmp)
{
	while (tmp && *tmp && expdr->size < expdr->total_size)
	{	
		expdr->expanded_var[expdr->size++] = *tmp;
		tmp++;
	}
}

void	init_expander_struct(t_expand *expdr, char *array, char *error_value, int mode)
{
	expdr->size = 0;
	expdr->current_quotes = '\0';
	expdr->array = array;
	expdr->mode = mode;
	expdr->error_value = error_value;
}