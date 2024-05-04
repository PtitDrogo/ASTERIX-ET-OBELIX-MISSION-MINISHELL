/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 19:40:15 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/04 22:19:53 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


void    syntax_error(t_token *token, t_garbage_collect *gc)
{
    // printf("hi first value of token is %s\n", token->str);
    while(token)
    {
        if (token->type != STR)
        {
            if (token->next == NULL || token->next->type != STR)
            {
                if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->next->str) == -1)
                    perror_exit(gc, errno, WRITE_ERR_MSG);
                empty_trash_exit(gc, errno);
            }
        }
        token = token->next;
    }
    return ;
}