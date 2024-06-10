/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_n_get.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:42:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/10 14:06:17 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_gc	**set_n_get_gc(t_gc **gc)
{
	static t_gc	**sgc;

	if (gc)
		sgc = gc;
	return (sgc);
}

t_cmd	*set_n_get_cmd(t_cmd *cmds)
{
	static t_cmd	*ccmds;

	if (cmds)
		ccmds = cmds;
	return (ccmds);
}

int	set_n_get_fd(int fd)
{
	static int	ffd;

	if (fd != -1)
		ffd = fd;
	return (ffd);
}
