/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:57:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/18 15:39:26 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isalpha(int c)
{
	return ((c >= 'a' && c <= 'z' ) || (c >= 'A' && c <= 'Z'));
}

//coding exit assuming I will get one string of words starting after the space
int ft_exit(char *args) //should take trash as argument too later;
{
    int exit_value;
    int i;

    //emptytrash here
    if (args[0] == '\0')
    {
		if (printf("exit\n") == -1)
			exit(EXIT_FAILURE);
		exit(0); // normal execution
	}
	if (is_letter_in_first_arg(args) == true)
	{
		if (printf("exit\nbash: exit: %s: numeric argument required", args) == -1) //TECHNICALLY NOT LIKE BASH BUT PAIN IN MY ASS
			exit(EXIT_FAILURE);
		exit(2);
	}
    
	i = 0;
    
    exit(123);
}
//error message should be written in 2 so the printf will have to go;
bool	is_letter_in_first_arg(char *str)
{
	int i;

	i = 0;
	while (str[i] && str[i] != ' ')
	{
		if (ft_isalpha(str[i]) == true)
			return (true);
		i++;
	}
	return (false);
}

bool	too_many_arguments(char *str)
{
	int i;
	i = 0;
	while (str[i])
	{
		i++;
	}
	
}


//if letter, return is 2,
//if too many arguments, return is 1
// if there is a letter in the first argument, it will be an error cuz letter no matter what
// if theres is no letter first argument and too many argument, error will be cuz too many arguments

//error because letter will still execute,
//error because too many argument doesnt executes;