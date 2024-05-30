/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_behavior.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 06:19:34 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 08:22:49 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int handle_first_pipe(t_token **token_current, int *tmp_fd, t_token **in);
int handle_in_redin(t_token **token_current, int *tmp_fd, t_token **in);
int handle_in_redout(t_token **token_current, int *tmp_fd, t_token **out);
int handle_dup(t_token **token_current, int *tmp_fd, t_token **in_out, int std);

int process_behavior(t_cmd *cmds, t_garbage_collect **gc, t_token *token_current)
{
	int		tmp_fd;
	int		status;

	status = 0;
    status = handle_first_pipe(&token_current, &tmp_fd, &cmds->redirection_in);
    if (status != 0)
        return (status);
	while (token_current)
	{
		if (token_current->type == D_LESS || token_current->type == LESS)
			status = handle_in_redin(&token_current, &tmp_fd, &cmds->redirection_in);
		else if (token_current->type == D_GREAT || token_current->type == GREAT || token_current->type == PIPE)
			status = handle_in_redout(&token_current, &tmp_fd, &cmds->redirection_out);
        if (status != 0)
                return (status);
		if (token_current->type == PIPE)
			break ;
		token_current = token_current->next;
	}
	return (0);
}

int handle_first_pipe(t_token **token_current, int *tmp_fd, t_token **in)
{
    if ((*token_current)->type == PIPE)
	{
		if (*in)
		{
			if ((*in)->type == PIPE)
			{	
				*tmp_fd = (*in)->pipe_fd;
				if (dup2(*tmp_fd, STDIN_FILENO) == -1)
				{	
                    if (ft_printf2("bash: Error Dupplicating file\n") == -1)
                        return (2);
                    return (1);
				}
			}
			(*in) = (*in)->next;
		}
		(*token_current) = (*token_current)->next;
	}
    return (0);
}

int handle_in_redin(t_token **token_current, int *tmp_fd, t_token **in)
{
    int status;
    
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
            *tmp_fd = (*in)->here_doc_pipe;
            (*in) = (*in)->next;
        }
        status = handle_dup(token_current, tmp_fd, in, STDIN_FILENO);
        (*in) = (*in)->next;
    }
    return (status);
}

int handle_in_redout(t_token **token_current, int *tmp_fd, t_token **out)
{
    int status;
    
    status = 0;
    if ((*out))
    {
        if ((*out)->type == GREAT)
        {	
            *tmp_fd = open((*out)->next->str, O_WRONLY | O_TRUNC | O_CREAT, 0644);
            if (*tmp_fd == -1)
                return (print_open_err_msg(errno, (*out)->next->str));
            (*out) = (*out)->next;
        }
        else if ((*out)->type == D_GREAT)
        {	
            *tmp_fd = open((*out)->next->str, O_WRONLY | O_APPEND | O_CREAT, 0644);
            if (*tmp_fd == -1)
                return (print_open_err_msg(errno, (*out)->next->str));
            (*out) = (*out)->next;
        }	
        else if ((*out)->type == PIPE)
            *tmp_fd = (*out)->pipe_fd;
        status = handle_dup(token_current, tmp_fd, out, STDOUT_FILENO);
        (*out) = (*out)->next;
    }
    return (status);
}

int handle_dup(t_token **token_current, int *tmp_fd, t_token **in_out, int std)
{
    if (((*in_out)->next == NULL) || (*in_out)->type == PIPE)
        if (dup2(*tmp_fd, std) == -1)
        {	
            if (ft_printf2("bash: Error Dupplicating file\n") == -1)
                return (2);
            return (1);
        }
    if ((*in_out)->type == LESS || (*in_out)->type == D_LESS)
        if (close(*tmp_fd) == -1)
        {	
            if (ft_printf2("Failed to close opened file")== -1)
                return (2);
            return (1);
        }
    return (0);
}
