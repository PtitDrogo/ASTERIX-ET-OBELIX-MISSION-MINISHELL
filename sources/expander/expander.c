/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 13:30:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/10 00:33:41 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//I coded this assuming Ill get the full thing with the dollar sign (exemple = $MIAOU)
//return the expanded string

//Bash essaye d'expand PUIS il vire les quotes;
//en theorie si j'enleve juste les quotes au debut et a la fin je suis gucci

int		count_new_size_of_array(char *array, t_env_node *env, t_garbage_collect **gc);
bool	can_expand(bool in_single_quotes, bool in_double_quotes);
char	*create_string_to_expand(char *str, t_garbage_collect **gc);
void	update_quotes_bools(char c, bool *in_single_quotes, bool *in_double_quotes);
char    *expand_var(t_env_node *env, t_garbage_collect **gc, char *to_expand);
char 	**expand(t_env_node *env, t_garbage_collect **gc, char **arrays);
int		chars_to_expand(char *str);


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
	in_single_quotes = false;
	in_double_quotes = false;
	//Try to expand first
	//then get rid of quotes;
	
	// printf("\n\n\n\n\n");
	// printf("I am feeding proper strings into this fucking function\n");
	// for (int test = 0; arrays[test]; test++)
	// {
	// 	printf("%s\n", arrays[test]);
	// }
	// printf("\n\n\n\n\n");

	while (arrays[i])
	{
		size = count_new_size_of_array(arrays[i], env, gc);
		// printf("after counting size, arrays[0] is %s and [1] is %s\n", arrays[0], arrays[1]);
		// write(1, "allo", 4);
		expanded_var = malloc_trash(size + 1, gc);
		expanded_var[size] = '\0';
		size = 0;
		j = 0;
		while (arrays[i][j])
		{
			// printf("hello I am in expand current char is %c\n", arrays[i][j]);
			if (arrays[i][j] == '\'' || arrays[i][j] == '\"')
				update_quotes_bools(arrays[i][j], &in_single_quotes, &in_double_quotes);
			// printf("about to check can expand, 1 quotes %i 2 quotes %i\n", in_single_quotes, in_double_quotes);
			if (arrays[i][j] == '$' && can_expand(in_single_quotes, in_double_quotes))
			{
				// printf("feeding into getenv %s\n", create_string_to_expand(&(arrays[i][j + 1]), gc));
				tmp = setter_gc(create_string_to_expand(&(arrays[i][j + 1]), gc), gc);
				if (ft_strlen(tmp) == 0)
				{	
					expanded_var[size++] = '$';
					j++;
				}
				else
				{
					j += ft_strlen(tmp) + 1; // + 1 maybe
					tmp = setter_gc(get_env_variable(env, tmp), gc);
					// printf("tmp, the thing getting the env variable is %s\n", tmp);
					while (tmp && *tmp)
					{	
						expanded_var[size++] = *tmp; //not ++j so we write over the $
						tmp++;
						// printf("array[i][j] == %c and tmp %c", arrays[i][j - 1], *(tmp - 1));
					}
				}
			}
			else	
				expanded_var[size++] = arrays[i][j++];
		}
		arrays[i] = expanded_var;
		i++;
	}
	// printf("I am returning arrays[0] : %s and expanded var arrays[1] : %s\n", arrays[0], arrays[1]);
	return (arrays);
}
//takes a string and return the size of the string if you replace the $VAR with their env variables;
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
			cur_var = create_string_to_expand(&array[i + 1], gc);
			if (ft_strlen(cur_var) == 0)
			{
				size++;
				i++;
			}
			else
			{
				size += ft_strlen(get_env_variable(env, cur_var)) - 1;
				i += ft_strlen(cur_var);
			}
		}
		else
		{	
			size++;
			i++;
		}
	}
	// printf("new size returns %i\n", size);
	return (size);
	//STEP 1 = JE CREE UNE STRING AVEC LE $ ET les chars jusqu'a un espace ou fin
	//STEP 1.5 = Je stock la taille de la str $TESTEST;
	//STEP 2 = Je compte la taille de la VAR ENV et je rajoute ca a size;
	//STEP 3 = je saute a l'indice i + size_of_var_name;
	//Je continue;
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

// int	remove_quotes(char **cmds)
// {
// 	int	i;
// 	int j;

// 	i = 0;

// 	while (cmds[i])
// 	{
// 		j = 0
// 		while()
// 	}


// }

