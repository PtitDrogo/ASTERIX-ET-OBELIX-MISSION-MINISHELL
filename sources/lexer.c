/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 18:15:09 by garivo            #+#    #+#             */
/*   Updated: 2024/04/24 19:03:24 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*TODO 
Lexer : 
Get input : Readline / split on whitespaces
Lexe : Tokenize every splitted input
Parse : Reorganize said tokens into commands*/

//#include "minishell_parsing.h"
#include "minishell.h"

t_token	*dup_token(t_token *token, t_garbage_collect **gc)
{
	t_token	*dup;

	if (!token)
		return (NULL);
	dup = malloc_trash(sizeof(t_token), gc);
	if (!dup)
		return (NULL);
	dup->str = setter_gc(ft_strdup(token->str), gc);
	if (!dup->str)
		empty_trash_exit(*gc, EXIT_FAILURE);
	dup->type = token->type;
	dup->next = NULL;
	return (dup);
}

void	add_token(t_token **tokenpile, t_token *new_token)
{
	t_token	*current;

	new_token->next = NULL;
	if (tokenpile && *tokenpile == NULL)
	{
		*tokenpile = new_token;
		return ;
	}
	current = *tokenpile;
	while (current->next)
		current = current->next;
	current->next = new_token;
	return ;
}

static t_token	*create_token(char *str, t_garbage_collect **gc)
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
	return (token);
}

t_token	*tokenize(char **input, t_garbage_collect **gc)
{
	t_token	*tokenpile;
	t_token	*token;
	int		i;

	i = 0;
	tokenpile = NULL;
	while (input[i])
	{
		token = create_token(input[i++], gc);
		//printf("token : %s\n", token->str);
		add_token(&tokenpile, token);
	}
	return (tokenpile);
}
