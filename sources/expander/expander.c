/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 08:22:41 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//I coded this assuming Ill get the full thing with the dollar sign (exemple = $MIAOU)
//return the expanded string

//Bash essaye d'expand PUIS il vire les quotes;
//en theorie si j'enleve juste les quotes au debut et a la fin je suis gucci

int		count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc, char *error_value);
bool	can_expand(char *current_quotes);
char	*create_string_to_expand(char *str, t_garbage_collect **gc);
char 	**expand(t_env_node *env, t_garbage_collect **gc, char **arrays, char *error_value);
int		chars_to_expand(char *str);
int		update_current_quote(char c, char *current_quotes);
char	**wrap_str_in_double_str(t_garbage_collect **gc, char *array);
char 	*expand_single_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value);
int		count_new_size_always_expand(char *array, t_env_node *env, t_garbage_collect **gc, char *error_value);

void	expander(t_env_node *env, t_garbage_collect **gc, t_cmd *cmds, char *error_value)
{
	t_token *current;
	
	while (cmds)
	{
		cmds->str = expand(env, gc, cmds->str, error_value);
		current = cmds->redirection_in;
		while (current)
		{
			current->str = expand_single_str(env, gc, current->str, error_value);
			current = current->next;
		}
		current = cmds->redirection_out;
		while (current)
		{
			current->str = expand_single_str(env, gc, current->str, error_value);
			current = current->next;
		}
		cmds = cmds->next;
	}
	return ;
}

char *expand_single_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value)
{
	int	i;
	int size;
	int total_size;
	char *expanded_var;
	char *tmp;
	i = 0;
	char current_quotes = '\0';
	if (array == NULL)
		return (NULL);

	total_size = count_new_size_of_array(array, env, gc, error_value);
	expanded_var = malloc_trash(total_size + 1, gc);
	expanded_var[total_size] = '\0';
	size = 0;
	while (array[i])
	{
		if (array[i] == '\'' || array[i] == '\"')
		{	
			if (update_current_quote(array[i], &current_quotes) == 1)
				expanded_var[size++] = array[i++];
			else
				i++;
		}
		else if (array[i] == '$' && can_expand(&current_quotes))
		{
			tmp = setter_gc(create_string_to_expand(&(array[i + 1]), gc), gc);
			if (ft_strlen(tmp) == 0)
			{	
				expanded_var[size++] = '$';
				i++;
			}
			else
			{
				i += ft_strlen(tmp) + 1;
				if (tmp && tmp[0] == '?')
					tmp = error_value;
				else
					tmp = setter_gc(get_env_variable(env, tmp), gc);
				while (tmp && *tmp && size < total_size)
				{	
					expanded_var[size++] = *tmp;
					tmp++;
				}
			}
		}
		else	
			expanded_var[size++] = array[i++];
	}
	return (expanded_var);
}

//Takes a double pointer and return its with ENV var expanded and quotes removed;
char **expand(t_env_node *env, t_garbage_collect **gc, char **arrays, char *error_value)
{
	int	i;
	int j;
	int size;
	
	char *expanded_var;
	char *tmp;
	i = 0;
	char current_quotes = '\0';
	if (arrays == NULL)
		return (NULL);
	while (arrays[i])
	{
		int total_size = count_new_size_of_array(arrays[i], env, gc, error_value);
		expanded_var = malloc_trash(total_size + 1, gc);
		expanded_var[total_size] = '\0';
		size = 0;
		j = 0;
		while (arrays[i][j])
		{
			if (arrays[i][j] == '\'' || arrays[i][j] == '\"')
			{	
				if (update_current_quote(arrays[i][j], &current_quotes) == 1)
					expanded_var[size++] = arrays[i][j++];
				else
					j++;
			}
			else if (arrays[i][j] == '$' && can_expand(&current_quotes))
			{
				tmp = setter_gc(create_string_to_expand(&(arrays[i][j + 1]), gc), gc);
				if (ft_strlen(tmp) == 0)
				{	
					expanded_var[size++] = '$';
					j++;
				}
				else
				{
					j += ft_strlen(tmp) + 1;
					if (tmp && tmp[0] == '?')
						tmp = error_value;
					else
						tmp = setter_gc(get_env_variable(env, tmp), gc);
					while (tmp && *tmp && size < total_size)
					{	
						expanded_var[size++] = *tmp;
						tmp++;
					}
				}
			}
			else	
				expanded_var[size++] = arrays[i][j++];
		}
		arrays[i] = expanded_var;
		i++;
		}
	return (arrays);
}
//Returns the size of a string after quotes removal and $ expansion;
int	count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc, char *error_value)
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
		if (array[i] == '\'' || array[i] == '\"')
		{	
			size += update_current_quote(array[i], &current_quotes); //absolute dark magic
			i++; 
		}
		else if (array[i] == '$' && can_expand(&current_quotes) == true)
		{
			cur_var = create_string_to_expand(&array[i + 1], gc);
			if (ft_strlen(cur_var) == 0)
			{
				size++;
				i++;
			}
			else if (array[i + 1] == '?') //We know previous char is $ and we know '?' stops expanding
			{
				size += ft_strlen(error_value); //Might as well give the string of error value already ?
				i += 2;
			}
			else
			{
				size += ft_strlen(get_env_variable(env, cur_var)) - 1; //Recursion baby (-1 for the $ sign)
				i += ft_strlen(cur_var); //if $ECHO, we jump by 4 characters;
			}
		}
		else
		{	
			size++;
			i++;
		}
	}
	return (size);
}

bool	can_expand(char *current_quotes)
{
	if (*current_quotes == '\'')
		return (false);
	else
		return (true);
}

//update current quotes and return the amount to increase size of string by;
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

//FEED INTO THIS THE CHAR AFTER THE DOLLAR (Maybe we change this later)
//This will check for every character after the dollar if the char is a valid char for env name
//when it meets a char thats not valid, it returns the string;
//If the very first char is not valid, it returns the string PLUS a dollar sign;
char	*create_string_to_expand(char *str, t_garbage_collect **gc)
{
	int 	i;
	int 	size;
	char	*new_str;
	
	size = chars_to_expand(str);
	new_str = malloc_trash(size + 1, gc);
	new_str[size] = '\0';
	i = -1;
	while(++i < size)
		new_str[i] = str[i];
	//if the len of this new string is 0 it means we keep the dollar, idk where to put that logic;
	return(new_str);
}

//Lets make a function that takes the string, starting after the dollar, and returns where
// the end of the expansion should occur;
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


char *remove_quotes(t_garbage_collect **gc, char *array)
{
	int	i;
	int size;
	int total_size;
	char *expanded_var;
	char *tmp;
	i = 0;
	char current_quotes = '\0';
	if (array == NULL)
		return (NULL);

	total_size = count_size_no_quotes(array, gc);
	expanded_var = malloc_trash(total_size + 1, gc);
	expanded_var[total_size] = '\0';
	size = 0;
	while (array[i])
	{
		if (array[i] == '\'' || array[i] == '\"')
		{	
			if (update_current_quote(array[i], &current_quotes) == 1)
				expanded_var[size++] = array[i++];
			else
				i++;
		}
		else	
			expanded_var[size++] = array[i++];
	}
	return (expanded_var);
}

int	count_size_no_quotes(char *array, t_garbage_collect **gc)
{
	int	i;
	int size;
	char current_quotes;

	i = 0;
	size = 0;
	current_quotes = '\0';
	if (array == NULL)
		return (0);
	while (array[i])
	{
		if (array[i] == '\'' || array[i] == '\"')
			size += update_current_quote(array[i], &current_quotes); //absolute dark magic
		else
			size++;
		i++;
	}
	return (size);
}

char *expand_here_doc_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value)
{
	int	i;
	int size;
	int total_size;
	char *expanded_var;
	char *tmp;
	char current_quotes;

	current_quotes = '\0';
	i = 0;
	if (array == NULL)
		return (NULL);

	total_size = count_new_size_always_expand(array, env, gc, error_value);
	expanded_var = malloc_trash(total_size + 1, gc);
	expanded_var[total_size] = '\0';
	size = 0;
	while (size < total_size)
	{
		if (array[i] == '\'' || array[i] == '\"')
		{	
			if (update_current_quote(array[i], &current_quotes) == 1)
				expanded_var[size++] = array[i++];
			else
				i++;
		}
		if (array[i] == '$')
		{
			tmp = setter_gc(create_string_to_expand(&(array[i + 1]), gc), gc);
			if (ft_strlen(tmp) == 0)
			{	
				expanded_var[size++] = '$';
				i++;
			}
			else
			{
				i += ft_strlen(tmp) + 1;
				if (tmp && tmp[0] == '?')
					tmp = error_value;
				else
					tmp = setter_gc(get_env_variable(env, tmp), gc);
				while (tmp && *tmp && size < total_size)
				{	
					expanded_var[size++] = *tmp;
					tmp++;
				}
			}
		}
		else	
			expanded_var[size++] = array[i++];
	}
	return (expanded_var);
}


int	count_new_size_always_expand(char *array, t_env_node *env, t_garbage_collect **gc, char *error_value)
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
		if (array[i] == '\'' || array[i] == '\"')
		{	
			size += update_current_quote(array[i], &current_quotes); //absolute dark magic
			i++; 
		}
		else if (array[i] == '$')
		{
			cur_var = create_string_to_expand(&array[i + 1], gc);
			if (ft_strlen(cur_var) == 0)
			{
				size++;
				i++;
			}
			else if (array[i + 1] == '?') //We know previous char is $ and we know '?' stops expanding
			{
				size += ft_strlen(error_value); //Might as well give the string of error value already ?
				i += 2;
			}
			else
			{
				size += ft_strlen(get_env_variable(env, cur_var)) - 1; //Recursion baby (-1 for the $ sign)
				i += ft_strlen(cur_var); //if $ECHO, we jump by 4 characters;
			}
		}
		else
		{	
			size++;
			i++;
		}
	}
	return (size);
}

