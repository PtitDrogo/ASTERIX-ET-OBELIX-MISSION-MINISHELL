/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 16:01:27 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 19:23:07 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	empty_trash_exit(t_gc *gc, int exit_code)
{
	int	i;

	i = 2;
	while (++i <= 1023)
		close(i);
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

void	exit_heredoc(int status)
{
	t_gc	**gc;

	gc = set_n_get_gc(NULL);
	close_all_heredoc_pipes(set_n_get_cmd(NULL), *gc);
	close(set_n_get_fd(-1));
	empty_trash_exit(*gc, status);
}

void	free_heredoc(void)
{
	t_gc	**gc;

	gc = set_n_get_gc(NULL);
	close_all_heredoc_pipes(set_n_get_cmd(NULL), *gc);
	close(set_n_get_fd(-1));
}
