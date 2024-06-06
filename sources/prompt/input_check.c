/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_check.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:24:53 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 12:25:11 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	is_ascii(unsigned char c);

int		verify_input(char *input)
{
	int i;
	
	i = 0;
	if (input[0] == '\0')
		return (0);
	while (input[i])
	{
		if (is_ascii((unsigned char)input[i]) == false)
			return (0);
		i++;
	}
	return (1);
}

static bool	is_ascii(unsigned char c)
{
	return (c <= 127);
}