/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 16:01:27 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 13:53:31 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	empty_trash_exit(t_gc *gc, int exit_code)
{
	empty_trash(gc);
	rl_clear_history();
	exit(exit_code);
	return ;
}

void	perror_exit(t_gc *gc, int exit_code, char *err_msg)
{
	perror(err_msg);
	exit_code = 1;
	empty_trash_exit(gc, exit_code);
	return ;
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
	t_gc	**gc;

	gc = global_gc(NULL);
	close_all_heredoc_pipes(global_cmd(NULL), *gc);
	close(global_fd(-1));
	empty_trash_exit(*gc, status);
}

void	free_heredoc(void)
{
	t_gc	**gc;

	gc = global_gc(NULL);
	close_all_heredoc_pipes(global_cmd(NULL), *gc);
	close(global_fd(-1));
}
