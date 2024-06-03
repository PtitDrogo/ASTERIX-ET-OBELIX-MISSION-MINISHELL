/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 18:15:09 by garivo            #+#    #+#             */
/*   Updated: 2024/06/03 06:28:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_to_last_redir(t_token **tokenpile)
{
	while (*tokenpile && (*tokenpile)->next && (*tokenpile)->next->next)
		*tokenpile = (*tokenpile)->next;
}

t_token	*dup_token(t_token *token, t_gc **gc)
{
	t_token	*dup;

	if (!token)
		return (NULL);
	dup = malloc_trash(sizeof(t_token), gc);
	dup->str = setter_gc(ft_strdup(token->str), gc);
	malloc_check(dup->str, *gc);
	dup->type = token->type;
	dup->token_fd = token->token_fd;
	dup->next = NULL;
	return (dup);
}

void	add_token(t_token **tokenpile, t_token *new_token)
{
	t_token	*current;

	new_token->next = NULL;
	if (tokenpile && *tokenpile == NULL)
	{
		new_token->prev = NULL;
		*tokenpile = new_token;
		return ;
	}
	current = *tokenpile;
	while (current->next)
		current = current->next;
	current->next = new_token;
	new_token->prev = current;
	return ;
}

static t_token	*create_token(char *str, t_gc **gc)
{
	t_token	*token;

	token = malloc_trash(sizeof(t_token), gc);
	if (!ft_strcmp(str, "|"))
		token->type = PIPE;
	else if (!ft_strcmp(str, ">"))
		token->type = GREAT;
	else if (!ft_strcmp(str, ">>"))
		token->type = D_GREAT;
	else if (!ft_strcmp(str, "<"))
		token->type = LESS;
	else if (!ft_strcmp(str, "<<"))
		token->type = D_LESS;
	else
		token->type = STR;
	token->str = str;
	token->token_fd = -1;
	return (token);
}

t_token	*tokenize(char **input, t_gc **gc)
{
	t_token	*tokenpile;
	t_token	*token;
	int		i;

	i = 0;
	tokenpile = NULL;
	// if (input == NULL);
	// 	return (NULL); //This gets rid of the memory leak but also nothing works so not great
	while (input[i])
	{
		token = create_token(input[i], gc);
		add_token(&tokenpile, token);
		i++;
	}
	return (tokenpile);
}