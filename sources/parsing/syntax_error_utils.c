/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 19:40:15 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 17:41:17 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	redir_next_not_str(t_token *token, t_gc *gc)
{
	if (token->next->type == PIPE)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n"
				, token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n"
				, token->next->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
}

bool	is_classic_redir_valid(t_token *token, t_gc *gc)
{
	if (token->next == NULL)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n"
				, token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	if (token->next->type != STR)
	{
		return (redir_next_not_str(token, gc), false);
	}
	if (token->next->type == STR && ft_strcmp(token->next->str, "\n") == 0)
	{
		if (ft_printf2("bash: syntax error near unexpected token `newline'\n"
				, token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	return (true);
}

bool	is_pipe_valid(t_token *token, t_gc *gc)
{
	if (token->next == NULL)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n"
				, token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	if (token->next->type == PIPE)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n"
				, token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	if (is_classic_redir(token->next))
	{
		if (is_classic_redir_valid(token->next, gc) == false)
			return (false);
	}
	return (true);
}

bool	is_classic_redir(t_token *token)
{
	t_tok_val	type;

	type = token->type;
	return (type == GREAT || type == D_GREAT || type == LESS || type == D_LESS);
}

bool	is_first_token_pipe(t_token *token, t_gc *gc)
{
	if (token && token->type == PIPE)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n"
				, token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (true);
	}
	return (false);
}
