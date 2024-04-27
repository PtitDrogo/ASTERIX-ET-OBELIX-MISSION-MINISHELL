/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 16:01:27 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/24 19:01:29 by garivo           ###   ########.fr       */
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

void    ft_error(char *error, t_garbage_collect *gc)
{
	if (error)
		printf("%s\n", error);
	/*if (gc)
	  empty_trash(gc);*/
}
