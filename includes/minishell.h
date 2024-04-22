/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:14:17 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/18 11:16:05 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

///------------------------Includes------------------------///
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>


///------------------------Structs------------------------///

typedef struct s_env_node {

	struct s_env_node	*next;
	char				*variable_name;
	char				*variable;
}	t_env_node;

typedef struct s_garbage_collect
{
	void						*to_free;
	struct s_garbage_collect	*next;
}	t_garbage_collect;

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

//garbage collector
int		add_to_trash(t_garbage_collect **root, void *to_free);
void    *malloc_trash(int size, t_garbage_collect **gc);
int 	empty_trash(t_garbage_collect *gc);
void	*setter_gc(void *data_to_set, t_garbage_collect **gc);
void    *setter_double_p_gc(void **data_to_set, t_garbage_collect **gc);

//BUILT INS
int	unset(t_env_node *env_dup_root, char *env_to_find);
int	export(t_env_node **root, void *variable, t_garbage_collect **gc);
int env(t_env_node *env_dup_root);

//UTILS
size_t	len_to_char(char *str, char c);
int	is_char_in_str(char *str, char c);
int	ft_strcmp(const char *s1, const char *s2);
int	pop(t_env_node *env_dup_root, t_env_node *node_to_pop);
int	generate_env_llist(t_env_node **env_dup_root, t_garbage_collect **gc, char **envp);

///------------------------Libft------------------------///
char	*get_next_line(int fd);
char	*ft_strdup(const char *src);
size_t	ft_strlen(const char *s);
char	*ft_strnstr(const char *big, const char *little, size_t len);
int	ft_isalpha(int c);
int	ft_isalnum(int c);
int	ft_isdigit(int c);
int	ft_strncmp(char *s1, char *s2, size_t n);
char	**ft_split(char const *s, char c);
void	ft_free_array(void **array);

///------------------------Parser/Lexer------------------------///
void	parse(char **input, t_garbage_collect **gc);
t_token	*tokenize(char **input, t_garbage_collect **gc);
void	add_token(t_token **tokenpile, t_token *new_token);
t_token	*dup_token(t_token *token, t_garbage_collect **gc);




////////////////////////////NOTES//////////////////////
/* Mot mot GREATER mot mot PIPE mot mot GREATER*/

/*hi access(get_env(CMD), F_OK); MIAOU,
//PREMIER MOT TJ UNE COMMANDE
// SI CA COMMENCE PAR UN TIRET CEST UN FLAG
//UNE FILE NE PEUT PAS COMMENCER PAR UN TIRET*/

/*// simple test that show that add history is cursed and will leak;
// int	main(void)
// {
// 	char* input;
	
// 	input = readline("myshell> ");
// 	add_history(input);
// 	rl_clear_history();
// 	free(input);
// 	// test();
// 	return (0);
*/

#endif
