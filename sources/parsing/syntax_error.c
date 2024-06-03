/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 19:40:15 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:28:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_classic_redir(t_token *token);
bool	is_classic_redir_valid(t_token *token, t_gc *gc);
bool	is_first_token_pipe(t_token *token, t_gc *gc);
bool	is_pipe_valid(t_token *token, t_gc *gc);

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

static void	redir_next_not_str(t_token *token, t_gc *gc)
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