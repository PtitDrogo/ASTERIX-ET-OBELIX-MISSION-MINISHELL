/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_parsing.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 19:06:58 by garivo            #+#    #+#             */
/*   Updated: 2024/04/17 22:32:30 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_PARSING_H
# define MINISHELL_PARSING_H

///------------------------Includes------------------------///

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>


///------------------------Structs------------------------///

typedef enum s_tok_val
{
	PIPE = 1,
	GREAT,
	D_GREAT,
	LESS,
	D_LESS,
	STR
}	t_tok_val;

typedef struct s_token
{
	char			*str;
	t_tok_val		type;
	struct s_token	*next;
}	t_token;

typedef struct s_cmd
{
	char					**str;
	//int						(*builtin)(t_tools *, struct s_simple_cmds *);
	int						num_redirections;
	char					*hd_file_name;
	t_token					*redirections;
	struct s_cmd			*next;
}	t_cmd;

///------------------------Functions------------------------///

t_token	*tokenize(char **input);
void	add_token(t_token **tokenpile, t_token *new_token);

#endif
