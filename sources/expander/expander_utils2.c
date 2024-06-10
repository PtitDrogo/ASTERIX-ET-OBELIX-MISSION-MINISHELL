/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 06:02:54 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/10 23:20:11 by tfreydie         ###   ########.fr       */
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

int	check_quotes(t_expand *ex, int *i)
{
	if (ex->array[*i] == '\'' || ex->array[*i] == '\"')
	{
		if (up_quote(ex->array[*i], &ex->quote) == 1 || ex->mode == EXPAND || ex->mode == STD_EX)
			ex->expanded_var[ex->size++] = ex->array[(*i)++];
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

void	init_expander_struct(t_expand *ex, char *array, char *error, int mode)
{
	ex->size = 0;
	ex->total_size = 0;
	ex->quote = '\0';
	ex->array = array;
	ex->mode = mode;
	ex->error_value = error;
	ex->in_here_doc = false;
	ex->end_here_doc = 0;
}
