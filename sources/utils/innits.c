/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   innits.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 15:58:57 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 18:47:16 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	innit_here_doc(t_gc **gc, t_cmd *cmd, int *status)
{
	*status = EXIT_SUCCESS;
	set_n_get_gc(gc);
	set_n_get_cmd(cmd);
}

int	minishell_graceful_exit(t_gc *gc)
{
	rl_clear_history();
	empty_trash(gc);
	return (0);
}
