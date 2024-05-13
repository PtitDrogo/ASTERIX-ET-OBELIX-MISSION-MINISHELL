/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 19:40:15 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/13 13:01:02 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool    is_classic_redir(t_token *token);

int     syntax_error(t_token *token, t_garbage_collect *gc)
{
    while(token)
    {
        if (is_classic_redir(token))
        {
            if (token->next == NULL)
            {
                if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
                    perror_exit(gc, errno, WRITE_ERR_MSG);
                return (1);
            }
            if (token->next->type != STR)
            {
                if (token->type == PIPE)
                {
                    if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
                        perror_exit(gc, errno, WRITE_ERR_MSG);
                }
                else
                {
                    if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->next->str) == -1)
                        perror_exit(gc, errno, WRITE_ERR_MSG);
                }
                return (1);
            }
            //doesnt work yet but newline should be taken into account at some point
            if (token->next->type == STR && ft_strcmp(token->next->str, "\n") == 0)
            {
                if (ft_printf_err("bash: syntax error near unexpected token `newline'\n", token->str) == -1)
                    perror_exit(gc, errno, WRITE_ERR_MSG);
                return (1);
            }
        }
        token = token->next;
        if (token->type == PIPE)
        {
            if (token->next == NULL)
            {
                if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
                    perror_exit(gc, errno, WRITE_ERR_MSG);
                return (1);
            }
            if (token->type == PIPE)
            {
                if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
                    perror_exit(gc, errno, WRITE_ERR_MSG);
                return (1);
            }
        }
    }
    return (0);
}

bool    is_classic_redir(t_token *token)
{
    t_tok_val type;

    type = token->type;

    return (type == GREAT || type == D_GREAT || type == LESS || type == D_LESS);
}