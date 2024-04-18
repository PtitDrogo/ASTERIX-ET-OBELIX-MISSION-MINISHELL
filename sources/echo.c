/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:40:04 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/18 14:57:43 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "minishell.h"
#include <stdbool.h>
#include <stdio.h>

char    *n_flag_logic(char *str, bool *n_flag);
//I want a function i can just dump all the word content to the
//right of echo and be happy with my life


//Ive coded echo assuming I would get the content of every "Word" to the right of it,
//that means flag and words to print all in one pointer, i can change that if need be
int echo(char *to_echo)
{
    bool    n_flag;
    
    to_echo = n_flag_logic(to_echo, &n_flag);
    
    printf("%s", to_echo);
    if (n_flag == false)    
        printf("\n");
    return (1);
}

//this function will update the bool pointer depending if its a valid flag
//it will return the pointer where we should start printing (as in, we dont print the flag !)
char    *n_flag_logic(char *str, bool *n_flag)
{
    int i;

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

//this code is a bit confusing but it copies bash behavior, -nnnnnnnnn is considered valid

// //test main
// int main(int argc, char *argv[])
// {
//     if (argc != 2)
//         return (1);
//     echo(argv[1]);
//     return 0;
// }
