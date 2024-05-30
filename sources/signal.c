/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 16:29:30 by garivo            #+#    #+#             */
/*   Updated: 2024/05/30 02:49:01 by garivo           ###   ########.fr       */
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

/*void	set_signal(int)
{
	struct sigaction	s_act;

	ft_bzero(&s_act, sizeof(s_act));
	s_act.sa_sigaction = &signal_handler;
	s_act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &s_act, NULL);
	sigaction(SIGQUIT, &s_act, NULL);
}*/
