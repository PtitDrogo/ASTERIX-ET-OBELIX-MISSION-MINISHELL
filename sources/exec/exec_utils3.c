/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 15:03:22 by garivo            #+#    #+#             */
/*   Updated: 2024/06/12 13:54:16 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	react_status(t_exec exec)
{
	if (exec.status == 130)
	{
		if (ft_printf("\n") == -1)
			return (-1);
	}
	if (exec.status == 131)
		if (ft_printf("Quit Core Dumped\n") == -1)
			return (-1);
	return (0);
}
