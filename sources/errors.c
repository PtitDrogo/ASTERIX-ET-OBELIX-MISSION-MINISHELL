/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 16:01:27 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/25 19:53:16 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//clear all memory allocation and exit shell with exit_code
void	empty_trash_exit(t_garbage_collect *gc, int exit_code)
{
	empty_trash(gc);
	rl_clear_history();
	exit(exit_code);
    return ;
}
//use this for failure of function that update errno (write, printf, dup etc)
void    perror_exit(t_garbage_collect *gc, int exit_code, char *err_msg)
{
    perror(err_msg);
    empty_trash_exit(gc, exit_code);
    return ;
}

