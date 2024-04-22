/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:48:23 by garivo            #+#    #+#             */
/*   Updated: 2024/04/22 17:46:34 by garivo           ###   ########.fr       */
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
	if (!new_list)
		return (NULL);
	i = 0;
	while (list && *list && (*list)[i])
	{
		new_list[i] = (char *)setter_gc(ft_strdup((*list)[i]), gc);
		if (!new_list[i++])
			return (NULL);
	}
	new_list[i++] = str;
	new_list[i] = NULL;
	*list = new_list; 
	return (new_list);
}

static t_cmd	*create_command(t_token **tokenpile, t_garbage_collect **gc)
{
	t_cmd	*cmd;
	t_token	*token;

	cmd = malloc_trash(sizeof(t_cmd), gc);
	if (!cmd)
		return (NULL);
	cmd->redirections = NULL;
	cmd->str = NULL;
	token = *tokenpile;
	while (token && token->type != PIPE)
	{
		if (token->type == GREAT || token->type == D_GREAT
			|| token->type == LESS || token->type == D_LESS)
		{
			add_token(&cmd->redirections, dup_token(token, gc));
			token = token->next;
			add_token(&cmd->redirections, dup_token(token, gc));
		}
		else
			add_str(&cmd->str, token->str, gc);
		token = token->next;
	}
	return (cmd);
}

void	parse(char **input, t_garbage_collect **gc)
{
	t_token	*tokenpile;
	t_token	*token;
	t_cmd	*cmd_chain;
	t_cmd	*cmd;
	int		start;

	cmd_chain = NULL;
	tokenpile = tokenize(input, gc);
	token = tokenpile;
	start = 1;
	while (token)
	{
		if (start)
		{
			cmd = create_command(&token, gc);
			add_command(&cmd_chain, cmd);
			start = 0;
		}
		if (token->type == PIPE)
			start = 1;
		token = token->next;
	}
	/*while (cmd_chain)
	{
		printf("new cmd :\n");
		int i = 0;
		while (cmd_chain->str && cmd_chain->str[i])
			printf("cmd : %s\n", cmd_chain->str[i++]);
		t_token	*redir = cmd->redirections;
		while (redir)
		{
			printf("redir : %s\n", redir->str);
			redir = redir->next;
		}
		cmd_chain = cmd_chain->next;
	}*/
}
