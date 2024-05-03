/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:48:23 by garivo            #+#    #+#             */
/*   Updated: 2024/05/03 17:00:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell_parsing.h"
#include "minishell.h"

static void	add_command(t_cmd **cmd_chain, t_cmd *new_cmd)
{
	t_cmd	*current;

	new_cmd->next = NULL;
	if (cmd_chain && *cmd_chain == NULL)
	{
		*cmd_chain = new_cmd;
		return ;
	}
	current = *cmd_chain;
	while (current->next)
		current = current->next;
	current->next = new_cmd;
	return ;
}

static char	**add_str(char ***list, char *str, t_garbage_collect **gc)
{
	char	**new_list;
	int		i;

	i = 0;
	while (list && *list && (*list)[i])
		i++;
	new_list = malloc_trash((i + 2) * sizeof(char *), gc);
	i = 0;
	while (list && *list && (*list)[i])
	{
		new_list[i] = (char *)setter_gc(ft_strdup((*list)[i]), gc);
		if (!new_list[i++])
			empty_trash_exit(*gc, EXIT_FAILURE);
	}
	new_list[i++] = str;
	new_list[i] = NULL;
	*list = new_list; 
	return (new_list);
}

static t_cmd	*create_command(t_token *tokenpile, t_garbage_collect **gc)
{
	t_cmd	*cmd;
	t_token	*token;
	t_token	**redir;

	cmd = malloc_trash(sizeof(t_cmd), gc);
	cmd->redirection_in = NULL;
	cmd->redirection_out = NULL;
	cmd->str = NULL;
	token = tokenpile;
	while (token && token->type != PIPE)
	{
		if (token->type == GREAT || token->type == D_GREAT
			|| token->type == LESS || token->type == D_LESS)
		{
			redir = &cmd->redirection_in;
			if (token->type == GREAT || token->type == D_GREAT)
				redir = &cmd->redirection_out;
			add_token(redir, dup_token(token, gc));
			token = token->next;
			add_token(redir, dup_token(token, gc));
		}
		else
			add_str(&cmd->str, token->str, gc);
		token = token->next;
	}
	set_to_last_redir(&cmd->redirection_in);
	set_to_last_redir(&cmd->redirection_out);
	if (token && token->type == PIPE && cmd->redirection_out == NULL)
		add_token(&cmd->redirection_out, dup_token(token, gc));
	return (cmd);
}
// DOESNT WORK WITH PIPE, ONE PIPE CAN BE BOTH REDIR IN AND REDIR OUT OF 2 CMD
// Return value The cmd list and a pointer that gets filled with the pipe number ?
//changed it so it fills pointers token and cmd given to it instead of doing it in the
//fucking void of space;
void	parse(char **input, t_garbage_collect **gc, t_token	**tokenpile, t_cmd	**cmd_chain)
{
	t_token	*token;
	t_cmd	*cmd;
	int		start;

	*cmd_chain = NULL;
	*tokenpile = tokenize(input, gc);
	token = *tokenpile;
	start = 1;
	while (token)
	{
		if (start)
		{
			cmd = create_command(token, gc);
			add_command(cmd_chain, cmd);
			start = 0;
		}
		if (token->type == PIPE)
			start = 1;
		token = token->next;
	}
	
	// while ((*cmd_chain))
	// {
	// 	printf("new cmd :\n");
	// 	int i = 0;
	// 	while ((*cmd_chain)->str && (*cmd_chain)->str[i])
	// 		printf("cmd : %s\n", (*cmd_chain)->str[i++]);
	// 	t_token	*redir_in = (*cmd_chain)->redirection_in;
	// 	t_token	*redir_out = (*cmd_chain)->redirection_out;
	// 	while (redir_in)
	// 	{
	// 		printf("redir_in : %s\n", redir_in->str);
	// 		redir_in = redir_in->next;
	// 	}
	// 	while (redir_out)
	// 	{
	// 		printf("redir_out : %s\n", redir_out->str);
	// 		redir_out = redir_out->next;
	// 	}
	// 	(*cmd_chain) = (*cmd_chain)->next;
	// }
}