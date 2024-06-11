/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 14:43:23 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_new_array_size(t_expand *expdr, t_env *env, t_gc **gc);
static void	fill_str(t_expand *expdr, t_gc **gc, t_env *env, int i);
static char	**expand(t_data *data, char **arrays, int mode);

void	expander(t_data *data)
{
	t_token	*current;
	t_cmd	*current_cmd;

	current_cmd = data->cmds;
	while (current_cmd)
	{
		current_cmd->str = expand(data, current_cmd->str, RMQUOTE);
		current = current_cmd->redirection_in;
		while (current)
		{
			current->str = expand_single_str(data,
					current->str, RMQUOTE);
			current = current->next;
		}
		current = current_cmd->redirection_out;
		while (current)
		{
			current->str = expand_single_str(data,
					current->str, RMQUOTE);
			current = current->next;
		}
		current_cmd = current_cmd->next;
	}
	return ;
}

char	*expand_single_str(t_data *data, char *array, int mode)
{
	t_expand	expdr;
	int			i;

	i = 0;
	if (array == NULL)
		return (NULL);
	ft_memset(&expdr, 0, sizeof(expdr));
	init_expander_struct(&expdr, array, data->str_status, mode);
	update_new_array_size(&expdr, data->env, &data->gc);
	expdr.expanded_var = malloc_trash(expdr.total_size + 1, &data->gc);
	expdr.expanded_var[expdr.total_size] = '\0';
	fill_str(&expdr, &data->gc, data->env, i);
	return (expdr.expanded_var);
}

static char	**expand(t_data *data, char **arrays, int mode)
{
	int			i;
	int			j;
	t_expand	expdr;

	i = 0;
	if (arrays == NULL)
		return (NULL);
	ft_memset(&expdr, 0, sizeof(expdr));
	init_expander_struct(&expdr, arrays[i], data->str_status, mode);
	while (arrays[i])
	{
		j = 0;
		expdr.array = arrays[i];
		expdr.size = 0;
		expdr.total_size = 0;
		update_new_array_size(&expdr, data->env, &data->gc);
		expdr.expanded_var = malloc_trash(expdr.total_size + 1, &data->gc);
		expdr.expanded_var[expdr.total_size] = '\0';
		fill_str(&expdr, &data->gc, data->env, j);
		arrays[i] = expdr.expanded_var;
		i++;
	}
	return (arrays);
}

static void	fill_str(t_expand *expdr, t_gc **gc, t_env *env, int i)
{
	char	*tmp;

	while (expdr->array[i])
	{
		handle_heredoc_case(expdr, i);
		if (check_quotes(expdr, &i))
			;
		else if (is_valid_dollar(expdr, i))
		{
			tmp = setter_gc(get_expand_str(&(expdr->array[i + 1]), gc), gc);
			if ((expdr->array[i + 1] == '\'' || expdr->array[i + 1] == '\"')
				&& expdr->quote == '\0')
				i++;
			else if (!tmp_check(expdr, &i, tmp))
			{
				if (!handle_question_mark(expdr, &i, &tmp))
					tmp = setter_gc(env_var(env, tmp), gc);
				fill_string(expdr, tmp);
			}
		}
		else
			expdr->expanded_var[expdr->size++] = expdr->array[i++];
	}
}

static void	update_new_array_size(t_expand *x, t_env *env, t_gc **gc)
{
	int		i;
	char	*cur_var;

	i = 0;
	if (x->array == NULL)
		return (x->total_size = 0, (void)0);
	while (x->array[i])
	{
		handle_heredoc_case(x, i);
		if ((x->array[i] == '\'' || x->array[i] == '\"' ) && x->mode == RMQUOTE)
			var_up(&x->total_size, &i, up_quote(x->array[i], &x->quote), 1);
		else if (is_valid_dollar(x, i))
		{
			cur_var = get_expand_str(&x->array[i + 1], gc);
			if (dollar_edge_cases(x, &i, cur_var))
				;
			else if (x->array[i + 1] == '?')
				var_up(&x->total_size, &i, ft_len(x->error_value), 2);
			else
				var_up(&x->total_size, &i,
					ft_len(env_var(env, cur_var)) - 1, ft_len(cur_var));
		}
		else
			var_up(&x->total_size, &i, 1, 1);
	}
}
