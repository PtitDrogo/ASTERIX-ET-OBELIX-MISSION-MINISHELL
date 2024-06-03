/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:48:23 by garivo            #+#    #+#             */
/*   Updated: 2024/06/03 06:28:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static char	**add_str(char ***list, char *str, t_gc **gc)
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

static void	fill_cmd(t_token **token, t_cmd *cmd, t_gc **gc)
{
	t_token	**redir;

	while (*token && (*token)->type != PIPE)
	{
		if ((*token)->type == GREAT || (*token)->type == D_GREAT
			|| (*token)->type == LESS || (*token)->type == D_LESS)
		{
			redir = &cmd->redirection_in;
			if ((*token)->type == GREAT || (*token)->type == D_GREAT)
				redir = &cmd->redirection_out;
			add_token(redir, dup_token(*token, gc));
			*token = (*token)->next;
			if (*token)
				add_token(redir, dup_token(*token, gc));
		}
		else
			add_str(&cmd->str, (*token)->str, gc);
		if (*token)
			*token = (*token)->next;
	}
}

static t_cmd	*create_command(t_token *tokenpile, t_gc **gc)
{
	t_cmd	*cmd;
	t_token	*token;

	cmd = malloc_trash(sizeof(t_cmd), gc);
	cmd->redirection_in = NULL;
	cmd->redirection_out = NULL;
	cmd->str = NULL;
	token = tokenpile;
	if (token && token->prev && token->prev->type == PIPE)
		add_token(&cmd->redirection_in, dup_token(token->prev, gc));
	fill_cmd(&token, cmd, gc);
	if (token && token->type == PIPE && cmd->redirection_out == NULL)
		add_token(&cmd->redirection_out, dup_token(token, gc));
	return (cmd);
}

int	parse(char **input, t_gc **gc,
	t_token	**tokenpile, t_cmd	**cmd_chain)
{
	t_token	*token;
	t_cmd	*cmd;
	int		start;

	*cmd_chain = NULL;
	*tokenpile = tokenize(input, gc);
	token = *tokenpile;
	start = 1;
	if (syntax_error(token, *gc) == 0) 
		return (0);
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
	return (1);
}
