/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 03:17:01 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc, char *error_value, int mode);
bool	can_expand(char *current_quotes);
char	*create_string_to_expand(char *str, t_garbage_collect **gc);
int		chars_to_expand(char *str);
int		update_current_quote(char c, char *current_quotes);
void	increase_two_vars(int *var_1, int *var_2, int add_to_1, int add_to_2);
void	fill_expanded_str(t_expand *expdr, t_garbage_collect **gc, t_env_node *env, int i);
void	fill_string(t_expand *expdr, char *tmp);
int		check_quotes(t_expand *expdr, int *i);
int		tmp_check(t_expand *expdr, int *i, char *tmp);
int		handle_question_mark(t_expand *expdr, int *i, char **tmp);

void	expander(t_env_node *env, t_garbage_collect **gc, t_cmd *cmds, char *error_value)
{
	t_token *current;
	
	while (cmds)
	{
		cmds->str = expand(env, gc, cmds->str, error_value, EXPAND_NORMAL);
		current = cmds->redirection_in;
		while (current)
		{
			current->str = expand_single_str(env, gc, current->str, error_value, EXPAND_NORMAL);
			current = current->next;
		}
		current = cmds->redirection_out;
		while (current)
		{
			current->str = expand_single_str(env, gc, current->str, error_value, EXPAND_NORMAL);
			current = current->next;
		}
		cmds = cmds->next;
	}
	return ;
}

void	init_expander_struct(t_expand *expdr, char *array, char *error_value, int mode)
{
	expdr->size = 0;
	expdr->current_quotes = '\0';
	expdr->array = array;
	expdr->mode = mode;
	expdr->error_value = error_value;
}

char *expand_single_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value, int mode)
{
	t_expand expdr;
	int i;
	
	i = 0;
	if (array == NULL)
		return (NULL);
	ft_memset(&expdr, 0, sizeof(expdr));
	expdr.total_size = count_new_size_of_array(array, env, gc, error_value, mode);
	expdr.expanded_var = malloc_trash(expdr.total_size + 1, gc);
	expdr.expanded_var[expdr.total_size] = '\0';
	init_expander_struct(&expdr, array, error_value, mode);
	fill_expanded_str(&expdr, gc, env, i);
	return (expdr.expanded_var);
}

char **expand(t_env_node *env, t_garbage_collect **gc, char **arrays, char *error_value, int mode)
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
		expdr.total_size = count_new_size_of_array(expdr.array, env, gc, error_value, mode);
		expdr.expanded_var = malloc_trash(expdr.total_size + 1, gc);
		expdr.expanded_var[expdr.total_size] = '\0';
		fill_expanded_str(&expdr, gc, env, j);
		arrays[i] = expdr.expanded_var;
		i++;
	}
	return (arrays);
}

void	fill_expanded_str(t_expand *expdr, t_garbage_collect **gc, t_env_node *env, int i)
{
	char	*tmp;

	while (expdr->array[i])
	{
		if (check_quotes(expdr, &i))
			;
		else if (expdr->array[i] == '$' && (can_expand(&expdr->current_quotes) == true || expdr->mode == ALWAYS_EXPAND) && expdr->mode != REMOVESQUOTES)
		{
			tmp = setter_gc(create_string_to_expand(&(expdr->array[i + 1]), gc), gc);
			if (tmp_check(expdr, &i, tmp))
				;
			else
			{
				if (handle_question_mark(expdr, &i, &tmp))
					;
				else
					tmp = setter_gc(get_env_variable(env, tmp), gc);
				fill_string(expdr, tmp);
			}
		}
		else	
			expdr->expanded_var[expdr->size++] = expdr->array[i++];
	}
}

int	handle_question_mark(t_expand *expdr, int *i, char **tmp)
{
	(*i) += ft_strlen(*tmp) + 1;
	if (*tmp && (*tmp)[0] == '?')
	{	
		*tmp = expdr->error_value;
		return (1);
	}
	return (0);
}
int	tmp_check(t_expand *expdr, int *i, char *tmp)
{
	if (ft_strlen(tmp) == 0)
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
		if (update_current_quote(expdr->array[*i], &expdr->current_quotes) == 1 || expdr->mode == ALWAYS_EXPAND)
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

int	count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc, char *error_value, int mode)
{
	int	i;
	int size;
	char *cur_var;
	char current_quotes;

	i = 0;
	size = 0;
	current_quotes = '\0';
	if (array == NULL)
		return (0);
	while (array[i])
	{
		if ((array[i] == '\'' || array[i] == '\"' ) && mode != ALWAYS_EXPAND)
			increase_two_vars(&size, &i, update_current_quote(array[i], &current_quotes), 1);
		else if (array[i] == '$' && (can_expand(&current_quotes) == true || mode == ALWAYS_EXPAND) && mode != REMOVESQUOTES)
		{
			cur_var = create_string_to_expand(&array[i + 1], gc);
			if (ft_strlen(cur_var) == 0)
				increase_two_vars(&size, &i, 1, 1);
			else if (array[i + 1] == '?')
				increase_two_vars(&size, &i, ft_strlen(error_value), 2);
			else
				increase_two_vars(&size, &i, ft_strlen(get_env_variable(env, cur_var)) - 1, ft_strlen(cur_var));
		}
		else
			increase_two_vars(&size, &i, 1, 1);
	}
	return (size);
}

void	increase_two_vars(int *var_1, int *var_2, int add_to_1, int add_to_2)
{
	*var_1 += add_to_1;
	*var_2 += add_to_2;
}

bool	can_expand(char *current_quotes)
{
	if (*current_quotes == '\'')
		return (false);
	else
		return (true);
}

int		update_current_quote(char c, char *current_quotes)
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

char	*create_string_to_expand(char *str, t_garbage_collect **gc)
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

