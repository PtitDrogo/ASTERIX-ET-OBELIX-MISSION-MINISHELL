/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:40:04 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 12:03:10 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static	char    *n_flag_logic(char *str, bool *n_flag);
static	char	*join_echo(char **to_echo, t_gc **gc);
static char *remove_white_spaces(char *str);

int echo(char **to_echo, t_gc **gc)
{
	bool    n_flag;
	bool	n_n_flag;
	char	*joined_string;


	int i = 0;
	
	// while (to_echo[i])
	// {
	// 	printf("fed into echo : %s\n", to_echo[i]);
	// 	i++;
	// }
	// printf("after print : %s\n", to_echo[i]);
	if (to_echo == NULL)
		return (1);
	if (to_echo[1] == NULL)
	{
		if (printf("\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		return (0);
	}
	// printf("toecho [1] == %s, to echo 2 = %s\n\n\n", to_echo[1],to_echo[2]);
	joined_string = join_echo(&to_echo[1], gc);
	joined_string = n_flag_logic(joined_string, &n_n_flag);
	n_flag = n_n_flag;
	while (n_n_flag == true)
		joined_string = n_flag_logic(joined_string, &n_n_flag);
	// I actually only want to remove white_space when echoing ENV variable, maybe add something in the env or in the expand about it ? idk
	// joined_string = remove_white_spaces(joined_string);
	if (printf("%s", joined_string) == -1)
		perror_exit(*gc, errno, WRITE_ERR_MSG);
	if (n_flag == false)    
	{
		if (printf("\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	return (0);
}
static	char	*join_echo(char **to_echo, t_gc **gc)
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
	{	
		// printf("i = %i\n", i);
		// printf("the word we are trying to get len is : %s\n", to_echo[i]);
		// for (j = 0; to_echo[i][j]; j++)
		// {
		// 	printf("char is : %c\n", to_echo[i][j]);
		// }
		letters += ft_len(to_echo[i++]);
	}
	letters += count_arrays_in_doubleptr((void **)to_echo) - 1; // this is for the space between words
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

//TODO, rewrite this so it works with -n -n -n -n -n and try not to break it with other use case this is brittle;
static	char    *n_flag_logic(char *str, bool *n_flag)
{
	int i;

	if (ft_len(str) < 2)
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
	if (str[i] == ' ')
		i++;
	return (&str[i]);
}

