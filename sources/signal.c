/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 16:29:30 by garivo            #+#    #+#             */
/*   Updated: 2024/05/31 18:59:11 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 600
#include <signal.h>
#include "minishell.h"

void	new_prompt(int none)
{
	(void)none;
	ft_printf("\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	cancel_cmd(int none)
{
	(void)none;
}

void	cancel_heredoc(int none)
{
	(void)none;
	exit_heredoc(130);
}
