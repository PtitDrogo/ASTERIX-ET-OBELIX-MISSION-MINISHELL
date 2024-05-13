/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 16:29:30 by garivo            #+#    #+#             */
/*   Updated: 2024/05/13 13:28:35 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 600
#include <signal.h>
#include "minishell.h"

volatile int	g_input = -1;

static void	signal_handler(int signum, siginfo_t *info, void *ucontext)
{
	(void)ucontext;
	ft_printf("\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay ();
	g_input = 12;
}

void	set_signal(void)
{
	struct sigaction	s_act;

	ft_bzero(&s_act, sizeof(s_act));
	s_act.sa_sigaction = &signal_handler;
	s_act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &s_act, NULL);
	sigaction(SIGQUIT, &s_act, NULL);
}
