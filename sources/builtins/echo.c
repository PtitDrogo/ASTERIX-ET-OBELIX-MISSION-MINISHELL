/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:40:04 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/24 14:14:58 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static	char    *n_flag_logic(char *str, bool *n_flag);
static	char	*join_echo(char **to_echo, t_garbage_collect **gc);
static char *remove_white_spaces(char *str);
//I want a function i can just dump all the word content to the
//right of echo and be happy with my life


//Ive coded echo assuming I would get the content of every "Word" to the right of it,
//that means flag and words to print all in one pointer, i can change that if need be

//TODO, add case for echo -n -n -n -nnnn -n hello world
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
	// printf("toecho [1] == %s, to echo 2 = %s\n\n\n", to_echo[1],to_echo[2]);
	joined_string = join_echo(&to_echo[1], gc);
	joined_string = n_flag_logic(joined_string, &n_flag);
	joined_string = remove_white_spaces(joined_string);
	if (printf("%s", joined_string) == -1)
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
	int 	j;
	char	*str_to_return;
	
	if (to_echo == NULL)
		return (NULL);
	i = 0;
	letters = 0;
	while (to_echo[i])
		letters += ft_strlen(to_echo[i++]);
	letters += count_arrays_in_doubleptr((void **)to_echo) - 1;
	str_to_return = malloc_trash(letters + 1, gc);
	str_to_return[letters] = '\0';
	i = 0;
	letters = 0;
	while (to_echo[i])
	{
		j = 0;
		while (to_echo[i][j])
			str_to_return[letters++] = to_echo[i][j++];
		i++;
		if (to_echo[i])
			str_to_return[letters++] = ' ';
	}
	// printf("join echo is returning %s\n", str_to_return);
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

static char *remove_white_spaces(char *str)
{
	int i;

	i = 0;
	while (str[i] >= 9 && str[i] <= 13 || str[i] == ' ')
		i++;
	return (&str[i]);
}

