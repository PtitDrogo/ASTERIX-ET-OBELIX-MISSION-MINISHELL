/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 19:40:15 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 05:33:14 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	is_classic_redir(t_token *token);
static bool	is_classic_redir_valid(t_token *token, t_garbage_collect *gc);
static bool	is_first_token_pipe(t_token *token, t_garbage_collect *gc);
static bool	is_pipe_valid(t_token *token, t_garbage_collect *gc);

int	syntax_error(t_token *token, t_garbage_collect *gc)
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

static bool	is_classic_redir_valid(t_token *token, t_garbage_collect *gc)
{
	int	err;

	err = -42;
	if (token->next == NULL)
		err = ft_printf2("bash: syntax error near unexpected token `newline'\n",
				token->str);
	else if (token->next->type != STR)
	{
		if (token->next->type == PIPE)
			err = ft_printf2("bash: syntax error near unexpected token `%s'\n",
					token->str);
		else
			err = ft_printf2("bash: syntax error near unexpected token `%s'\n",
					token->next->str);
	}
	else
		return (true);
	if (err == -1)
		perror_exit(gc, errno, WRITE_ERR_MSG);
	return (false);
}

static bool	is_pipe_valid(t_token *token, t_garbage_collect *gc)
{
	if (token->next == NULL)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n",
				token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	if (token->next->type == PIPE)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n",
				token->str) == -1)
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

static bool	is_classic_redir(t_token *token)
{
	t_tok_val	type;

	type = token->type;
	return (type == GREAT || type == D_GREAT || type == LESS || type == D_LESS);
}

static bool	is_first_token_pipe(t_token *token, t_garbage_collect *gc)
{
	if (token && token->type == PIPE)
	{
		if (ft_printf2("bash: syntax error near unexpected token `%s'\n",
				token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (true);
	}
	return (false);
}
