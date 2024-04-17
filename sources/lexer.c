/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 18:15:09 by garivo            #+#    #+#             */
/*   Updated: 2024/04/17 21:27:07 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*TODO 
Lexer : 
Get input : Readline / split on whitespaces
Lexe : Tokenize every splitted input
Parse : Reorganize said tokens into commands*/

#include "minishell_parsing.h"
#include "minishell.h"

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

static t_token	*create_token(char *str)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
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

t_token	*tokenize(char **input)
{
	printf("Lexing...\n");
	t_token	*tokenpile;
	t_token	*token;
	int		i;

	i = 0;
	tokenpile = NULL;
	while (input[i])
	{
		token = create_token(input[i++]);
		printf("%s\n", token->str);
		add_token(&tokenpile, token);
	}
	return (tokenpile);
}
