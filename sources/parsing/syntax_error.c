/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 19:40:15 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/21 13:07:25 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_classic_redir(t_token *token);
bool	is_classic_redir_valid(t_token *token, t_garbage_collect *gc);
bool	is_first_token_pipe(t_token *token, t_garbage_collect *gc);
bool	is_pipe_valid(t_token *token, t_garbage_collect *gc);

//Pour etre clean faire une fonction qui call les here doc avant de return
// false;
int     syntax_error(t_token *token, t_garbage_collect *gc)
{
	if (is_first_token_pipe(token, gc) == true)
		return (0);
	while(token)
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

bool is_classic_redir_valid(t_token *token, t_garbage_collect *gc)
{
	if (token->next == NULL)
	{
		if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	if (token->next->type != STR)
	{
		// check for "< |", bash blames the pipe in err msg
		if (token->next->type == PIPE)
		{
			if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
				perror_exit(gc, errno, WRITE_ERR_MSG);
		}
		// check for "< <" or "<< >>", bash blames the 2nd redir
		else
		{
			if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->next->str) == -1)
				perror_exit(gc, errno, WRITE_ERR_MSG);
		}
		return (false);
	}
	//bash blames the \n sometime so edge case for it
	if (token->next->type == STR && ft_strcmp(token->next->str, "\n") == 0)
	{
		if (ft_printf_err("bash: syntax error near unexpected token `newline'\n", token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	return (true);	
}

bool is_pipe_valid(t_token *token, t_garbage_collect *gc)
{
	if (token->next == NULL)
	{
		if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (false);
	}
	if (token->next->type == PIPE)
	{
		if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
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

bool    is_classic_redir(t_token *token)
{
	t_tok_val type;

	type = token->type;

	return (type == GREAT || type == D_GREAT || type == LESS || type == D_LESS);
}

bool is_first_token_pipe(t_token *token, t_garbage_collect *gc)
{
	if (token && token->type == PIPE)
	{
		if (ft_printf_err("bash: syntax error near unexpected token `%s'\n", token->str) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (true);
	}
	return (false);
}