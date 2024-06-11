/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_exp.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 21:56:24 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 14:42:25 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_to_end_of_heredoc(t_expand *x, int i)
{
	char	end_goal;

	end_goal = ' ';
	if (x->array[i])
		i++;
	while (x->array[i] && x->array[i] == ' ')
		i++;
	if (x->array[i] == '\'' || x->array[i] == '\"')
		end_goal = x->array[i++];
	while (x->array[i] && x->array[i] != end_goal)
	{
		i++;
	}
	return (i);
}

void	handle_heredoc_case(t_expand *x, int i)
{
	if (x->array[i] == '<' && x->array[i + 1] == '<' && x->quote == '\0')
	{
		x->in_here_doc = true;
		x->end_here_doc = get_to_end_of_heredoc(x, i + 1);
	}
	if (x->in_here_doc == true && i == x->end_here_doc)
	{
		x->in_here_doc = false;
		x->end_here_doc = 0;
	}
}

bool	is_valid_dollar(t_expand *x, int i)
{
	return (x->array[i] == '$' && (can_expand(&x->quote) == true
			|| x->mode == EXPAND) && x->mode != RMQUOTE
		&& x->in_here_doc == false);
}

int	dollar_edge_cases(t_expand *x, int *i, char *cur_var)
{
	if ((x->array[(*i) + 1] == '\'' || x->array[(*i) + 1] == '\"')
		&& x->quote == '\0')
	{
		(*i)++;
		return (1);
	}
	else if (ft_len(cur_var) == 0)
	{
		var_up(&x->total_size, i, 1, 1);
		return (1);
	}
	return (0);
}
