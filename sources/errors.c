/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 16:01:27 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 06:24:52 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//clear all memory allocation and exit shell with exit_code
void	empty_trash_exit(t_garbage_collect *gc, int exit_code)
{
	empty_trash(gc);
	rl_clear_history();
	// printf("Exit code is %i\n", exit_code);
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
//TODO replace with with my error;
void    ft_error(char *error, t_garbage_collect *gc)
{
	if (error)
		printf("%s\n", error);
	/*if (gc)
	  empty_trash(gc);*/
}

int	exit_status(int status)
{
	static int	exit_status;

	if (status != -1)
		exit_status = status;
	return (exit_status);
}

void	exit_heredoc(int status)
{
	t_garbage_collect	**gc;

	gc = global_gc(NULL);
	close_all_heredoc_pipes(global_cmd(NULL), *gc);
	close(global_fd(-1));
	empty_trash_exit(*gc, status);
}

void	free_heredoc(void)
{
	t_garbage_collect	**gc;

	gc = global_gc(NULL);
	close_all_heredoc_pipes(global_cmd(NULL), *gc);
	close(global_fd(-1));
}
