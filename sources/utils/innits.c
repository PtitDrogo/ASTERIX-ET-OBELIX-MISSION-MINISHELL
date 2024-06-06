/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   innits.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 15:58:57 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 12:53:55 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	before_expand_innit(t_data *data)
{
	signal(SIGINT, cancel_cmd);
	signal(SIGQUIT, cancel_cmd);
	data->str_status = ft_itoa(exit_status(-1));
	setter_gc(data->str_status, &data->gc);
	malloc_check(data->str_status, data->gc);
}

void	innit_here_doc(t_gc **gc, t_cmd *cmd, int *status)
{
	*status = EXIT_SUCCESS;
	global_gc(gc);
	global_cmd(cmd);
}

void	minishell_graceful_exit(t_gc *gc)
{
	ft_printf("exit\n");
	rl_clear_history();
	empty_trash(gc);
}
