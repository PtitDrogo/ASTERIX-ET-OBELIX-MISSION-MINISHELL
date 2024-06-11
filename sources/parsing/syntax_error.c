/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 17:38:47 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/10 23:17:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	syntax_error(t_token *token, t_gc *gc)
{
	if (is_first_token_pipe(token, gc) == true)
		return (0);
	while (token)
	{
		if (is_classic_redir(token))
		{
			if (is_classic_redir_valid(token, gc) == false)
				return (0);
		}
		if (token->type == PIPE)
		{
			if (is_pipe_valid(token, gc) == false)
				return (0);
		}
		token = token->next;
	}
	return (1);
}
