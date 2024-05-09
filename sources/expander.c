/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/09 14:16:42 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//I coded this assuming Ill get the full thing with the dollar sign (exemple = $MIAOU)
//return the expanded string

//Bash essaye d'expand PUIS il vire les quotes;
//en theorie si j'enleve juste les quotes au debut et a la fin je suis gucci

int		count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc);
bool	can_expand(bool in_single_quotes, bool in_double_quotes);
char	*create_string_until_space(char *str, t_garbage_collect **gc);
void	update_quotes_bools(char c, bool *in_single_quotes, bool *in_double_quotes);
char    *expand_var(t_env_node *env, t_garbage_collect **gc, char *to_expand);
char **expand(t_env_node *env, t_garbage_collect **gc, char **arrays);


void	expander(t_env_node *env, t_garbage_collect **gc, t_cmd *cmds)
{
	while (cmds)
	{
		cmds->str = expand(env, gc, cmds->str);
		// remove_quotes();
		cmds = cmds->next;
	}
	return ;
}

//not using this atm but maybe i will eventually
char    *expand_var(t_env_node *env, t_garbage_collect **gc, char *to_expand)
{
	char    *str_to_return;
	
	to_expand = &to_expand[1]; //small ass line to go past the $ 
	if (!env || !to_expand)
		return (NULL);
	while (env) 
	{
		if (env->variable_name && ft_strcmp(env->variable_name, to_expand) == 0)
		{
			if (env->variable == NULL)
				return (NULL);
			str_to_return = setter_gc(ft_strdup(env->variable), gc);
			malloc_check(str_to_return, *gc);
			return (str_to_return);
		}
		env = env->next;
	}
	return (NULL);
}

//Takes a double pointer and return its with ENV var expanded and quotes removed;
char **expand(t_env_node *env, t_garbage_collect **gc, char **arrays)
{
	int	i;
	int j;
	int size;
	
	bool in_single_quotes;
	bool in_double_quotes;
	char *expanded_var;
	char *tmp;
	i = 0;
	j = 0;
	in_single_quotes = false;
	in_double_quotes = false;
	//Try to expand first
	//then get rid of quotes;
	
	while (arrays[i])
	{
		size = count_new_size_of_array(arrays[i], env, gc);
		expanded_var = malloc_trash(size + 1, gc);
		size = 0;
		while (arrays[i][j])
		{
			if (arrays[i][j] == '\'' || arrays[i][j] == '\"')
				update_quotes_bools(arrays[i][j], &in_single_quotes, &in_double_quotes);
			if (arrays[i][j] == '$' && can_expand(in_single_quotes, in_double_quotes))
			{
				tmp = get_env_variable(env, create_string_until_space(&arrays[i][j], gc));
				while (tmp)
					arrays[i][j++] = *tmp++; //not ++j so we write over the $
				arrays[i][j] = '\0';
			}
			expanded_var[size] = arrays[i][j];
			j++;
		}
		i++;
	}
	return (arrays);
}

int	count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc)
{
	int	i;
	int size;
	char *cur_var;
	bool in_single_quotes;
	bool in_double_quotes;

	i = 0;
	size = 0;
	in_single_quotes = false;
	in_double_quotes = false;
	while (array[i])
	{
		if (array[i] == '\'' || array[i] == '\"')
			update_quotes_bools(array[i], &in_single_quotes, &in_double_quotes);
		if (array[i] == '$' && can_expand(in_single_quotes, in_double_quotes) == true)
		{
			cur_var = create_string_until_space(&array[i], gc);
			size += ft_strlen(get_env_variable(env, cur_var)) - 1; //substract $ and add VAR
			i += ft_strlen(cur_var) - 1;
			//STEP 1 = JE CREE UNE STRING AVEC LE $ ET les chars jusqu'a un espace ou fin
			//STEP 1.5 = Je stock la taille de la str $TESTEST;
			//STEP 2 = Je compte la taille de la VAR ENV et je rajoute ca a size;
			//STEP 3 = je saute a l'indice i + size_of_var_name;
			//Je continue;	
		}
		else
		{
			i++;
			size++;
		}
	}
	return (size);
}

bool	can_expand(bool in_single_quotes, bool in_double_quotes)
{
	if (in_double_quotes == false && in_single_quotes == true)
		return (false);
	else
		return (true);
}


void	update_quotes_bools(char c, bool *in_single_quotes, bool *in_double_quotes)
{
	if (c == '\'')
		*in_single_quotes = !(*in_single_quotes); // clever trick to switch on/off
	if (c == '\"')
		*in_double_quotes = !(*in_double_quotes);	
	return ;
}

char	*create_string_until_space(char *str, t_garbage_collect **gc)
{
	int 	i;
	int 	size;
	char	*new_str;
	
	i = 0;
	while(str[i] && str[i] != ' ')
		i++;
	size = i;
	new_str = malloc_trash(size + 1, gc);
	new_str[size] = '\0';
	i = -1;
	while(++i < size)
		new_str[i] = str[i];
	return(new_str);
}

