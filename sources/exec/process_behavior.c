/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_behavior.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 06:19:34 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/10 16:34:46 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_first_pipe(t_token **token_cur, int *tmp_fd, t_token **in);
int	handle_in_redin(t_token **token_cur, int *tmp_fd, t_token **in);
int	handle_in_redout(t_token **token_cur, int *tmp_fd, t_token **out);
int	handle_dup(t_token **token_cur, int *tmp_fd, t_token **in_out, int std);

int	process_behavior(t_cmd *cmds, t_token *current)
{
	int		tmp_fd;
	int		err;

	err = 0;
	err = handle_first_pipe(&current, &tmp_fd, &cmds->redirection_in);
	if (err != 0)
		return (err);
	while (current)
	{
		if (current->type == D_LESS || current->type == LESS)
			err = handle_in_redin(&current, &tmp_fd, &cmds->redirection_in);
		else if (current->type == D_GREAT || current->type == GREAT
			|| current->type == PIPE)
			err = handle_in_redout(&current, &tmp_fd, &cmds->redirection_out);
		if (err != 0)
			return (err);
		if (current->type == PIPE)
			break ;
		current = current->next;
	}
	return (0);
}

int	handle_first_pipe(t_token **token_cur, int *tmp_fd, t_token **in)
{
	if ((*token_cur)->type == PIPE)
	{
		if (*in)
		{
			if ((*in)->type == PIPE)
			{
				*tmp_fd = (*in)->token_fd;
				if (dup2(*tmp_fd, STDIN_FILENO) == -1)
				{
					if (ft_printf2("bash: Error Dupplicating file\n") == -1)
						return (2);
					return (1);
				}
			}
			(*in) = (*in)->next;
		}
		(*token_cur) = (*token_cur)->next;
	}
	return (0);
}

int	handle_in_redin(t_token **token_cur, int *tmp_fd, t_token **in)
{
	int	status;

	status = 0;
	if ((*in))
	{
		if ((*in)->type == LESS)
		{
			*tmp_fd = open((*in)->next->str, O_RDONLY);
			if (*tmp_fd == -1)
				return (print_open_err_msg(errno, (*in)->next->str));
			(*in) = (*in)->next;
		}
		else if ((*in)->type == D_LESS)
		{
			*tmp_fd = (*in)->token_fd;
			(*in) = (*in)->next;
		}
		status = handle_dup(token_cur, tmp_fd, in, STDIN_FILENO);
		(*in) = (*in)->next;
	}
	return (status);
}

int	handle_in_redout(t_token **token_cur, int *fd, t_token **out)
{
	int	status;

	status = 0;
	if ((*out))
	{
		if ((*out)->type == GREAT)
		{
			*fd = open((*out)->next->str, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			if (*fd == -1)
				return (print_open_err_msg(errno, (*out)->next->str));
			(*out) = (*out)->next;
		}
		if ((*out)->type == D_GREAT)
		{
			*fd = open((*out)->next->str, O_WRONLY | O_APPEND | O_CREAT, 0644);
			if (*fd == -1)
				return (print_open_err_msg(errno, (*out)->next->str));
			(*out) = (*out)->next;
		}
		if ((*out)->type == PIPE)
			*fd = (*out)->token_fd;
		status = handle_dup(token_cur, fd, out, STDOUT_FILENO);
		(*out) = (*out)->next;
	}
	return (status);
}

int	handle_dup(t_token **token_cur, int *tmp_fd, t_token **in_out, int std)
{
	if (((*in_out)->next == NULL) || (*in_out)->type == PIPE)
	{
		if (dup2(*tmp_fd, std) == -1)
		{
			if (ft_printf2("bash: Error Dupplicating file\n") == -1)
				return (2);
			return (1);
		}
	}
	if ((*token_cur)->type == LESS || (*token_cur)->type == D_LESS
		|| (*token_cur)->type == GREAT || (*token_cur)->type == D_GREAT)
	{
		if (close(*tmp_fd) == -1)
		{
			if (ft_printf2("Failed to close opened file") == -1)
				return (2);
			return (1);
		}
	}
	return (0);
}
