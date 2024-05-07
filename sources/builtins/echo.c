/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:40:04 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/07 20:00:54 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
// #include <stdbool.h>
// #include <stdio.h>

static	char    *n_flag_logic(char *str, bool *n_flag);
static	char	*join_echo(char **to_echo, t_garbage_collect **gc);
//I want a function i can just dump all the word content to the
//right of echo and be happy with my life


//Ive coded echo assuming I would get the content of every "Word" to the right of it,
//that means flag and words to print all in one pointer, i can change that if need be

//This is coded like trash im touching it again when im sure of the type
//of input i will get.
int echo(char **to_echo, t_garbage_collect **gc)
{
	bool    n_flag;
	char	*joined_string;
	
	if (to_echo == NULL)
		return (1);
	if (to_echo[1] == NULL)
	{
		if (printf("\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		return (1);
	}
	//Need to string join all the strings into one actually
	joined_string = join_echo(&to_echo[1], gc);
	joined_string = n_flag_logic(joined_string, &n_flag);
	
	if (printf("%s", to_echo) == -1)
		perror_exit(*gc, errno, WRITE_ERR_MSG);
	if (n_flag == false)    
	{
		if (printf("\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	return (1);
}
static	char	*join_echo(char **to_echo, t_garbage_collect **gc)
{
	int		letters;
	int		i;
	char	*str_to_return;
	
	if (to_echo == NULL)
		return (NULL);
	i = 0;
	letters = 0;

	while (to_echo[i])
	{
		letters += ft_strlen(to_echo[i]);
		i++;
	}
	str_to_return = malloc_trash(letters, gc);
	str_to_return[letters] = '\0';
	i = 0;
	letters = 0;
	int j;
	while (to_echo[i])
	{
		j = 0;
		while (to_echo[i][j])
			str_to_return[letters++] = to_echo[i][j++];
		i++;
	}
	return (str_to_return);

	
}





//this function will update the bool pointer depending if its a valid flag
//it will return the pointer where we should start printing (as in, we dont print the flag !)
static	char    *n_flag_logic(char *str, bool *n_flag)
{
	int i;
	
	if (ft_strlen(str) <= 2)
	{	
		*n_flag = false;
		return (str);
	}
	if (str[0] != '-' && str[1] != 'n')
	{
		*n_flag = false;
		return (str);
	}
	i = 2;
	while (str[i] && str[i] != ' ')
	{
		if (str[i] != 'n')
		{
			*n_flag = false;
			return (str);
		}
		i++;
	}
	*n_flag = true;
	return (&str[++i]);
}

