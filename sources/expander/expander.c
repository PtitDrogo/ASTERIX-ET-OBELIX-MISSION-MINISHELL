/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:39:02 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	update_new_array_size(t_expand *expdr, t_env_node *env, t_gc **gc);
static void	fill_expanded_str(t_expand *expdr, t_gc **gc, t_env_node *env, int i);
static char **expand(t_env_node *env, t_gc **gc, char **arrays, char *error_value, int mode);

void	expander(t_env_node *env, t_gc **gc, t_cmd *cmds, char *error_value)
{
	t_token *current;
	
	while (cmds)
	{
		cmds->str = expand(env, gc, cmds->str, error_value, STD_EX);
		current = cmds->redirection_in;
		while (current)
		{
			current->str = expand_single_str(env, gc, current->str, error_value, STD_EX);
			current = current->next;
		}
		current = cmds->redirection_out;
		while (current)
		{
			current->str = expand_single_str(env, gc, current->str, error_value, STD_EX);
			current = current->next;
		}
		cmds = cmds->next;
	}
	return ;
}

char *expand_single_str(t_env_node *env, t_gc **gc, char *array, char *error_value, int mode)
{
	t_expand expdr;
	int i;
	
	i = 0;
	if (array == NULL)
		return (NULL);
	ft_memset(&expdr, 0, sizeof(expdr));
	init_expander_struct(&expdr, array, error_value, mode);
	expdr.total_size = update_new_array_size(&expdr, env, gc);
	expdr.expanded_var = malloc_trash(expdr.total_size + 1, gc);
	expdr.expanded_var[expdr.total_size] = '\0';
	fill_expanded_str(&expdr, gc, env, i);
	return (expdr.expanded_var);
}

static char **expand(t_env_node *env, t_gc **gc, char **arrays, char *error_value, int mode)
{
	int	i;
	int j;
	t_expand expdr;
	
	i = 0;
	if (arrays == NULL)
		return (NULL);
	ft_memset(&expdr, 0, sizeof(expdr));
	init_expander_struct(&expdr, arrays[i], error_value, mode);
	while (arrays[i])
	{
		j = 0;
		expdr.array = arrays[i];
		expdr.size = 0;
		expdr.total_size = update_new_array_size(&expdr, env, gc);
		expdr.expanded_var = malloc_trash(expdr.total_size + 1, gc);
		expdr.expanded_var[expdr.total_size] = '\0';
		fill_expanded_str(&expdr, gc, env, j);
		arrays[i] = expdr.expanded_var;
		i++;
	}
	return (arrays);
}

static void	fill_expanded_str(t_expand *expdr, t_gc **gc, t_env_node *env, int i)
{
	char	*tmp;

	while (expdr->array[i])
	{
		if (check_quotes(expdr, &i))
			;
		else if (expdr->array[i] == '$' && (can_expand(&expdr->current_quotes) == true
			|| expdr->mode == EXPAND) && expdr->mode != REMOVESQUOTES)
		{
			tmp = setter_gc(create_string_to_expand(&(expdr->array[i + 1]), gc), gc);
			if (tmp_check(expdr, &i, tmp))
				;
			else
			{
				if (handle_question_mark(expdr, &i, &tmp))
					;
				else
					tmp = setter_gc(env_var(env, tmp), gc);
				fill_string(expdr, tmp);
			}
		}
		else	
			expdr->expanded_var[expdr->size++] = expdr->array[i++];
	}
}

static int	update_new_array_size(t_expand *ex, t_env_node *env, t_gc **gc)
{
	int	i;
	int total_size;
	char *cur_var;

	i = 0;
	total_size = 0;
	if (ex->array == NULL)
		return (0);
	while (ex->array[i])
	{
		if ((ex->array[i] == '\'' || ex->array[i] == '\"' ) && ex->mode != EXPAND)
			var_up(&total_size, &i, up_quote(ex->array[i], &ex->current_quotes), 1);
		else if (ex->array[i] == '$' && (can_expand(&ex->current_quotes) == true 
			|| ex->mode == EXPAND) && ex->mode != REMOVESQUOTES)
		{
			cur_var = create_string_to_expand(&ex->array[i + 1], gc);
			if (ft_len(cur_var) == 0)
				var_up(&total_size, &i, 1, 1);
			else if (ex->array[i + 1] == '?')
				var_up(&total_size, &i, ft_len(ex->error_value), 2);
			else
				var_up(&total_size, &i, ft_len(env_var(env, cur_var)) - 1, ft_len(cur_var));
		}
		else
			var_up(&total_size, &i, 1, 1);
	}
	return (total_size);
}
