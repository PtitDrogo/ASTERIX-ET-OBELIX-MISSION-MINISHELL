/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:40:04 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/16 17:44:35 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int echo(char *to_echo, bool n_flag)
{
    printf("%s", to_echo);
    if (n_flag == false)    
        printf("\n");
    return (1);
}