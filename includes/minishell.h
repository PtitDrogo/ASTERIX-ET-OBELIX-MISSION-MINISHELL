/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:14:17 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/30 00:36:43 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

///------------------------Includes------------------------///
# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h> 
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <errno.h>
# include "../libft/includes/libft.h"


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
	
} t_garbage_collect;

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
	t_token					*redirection_in;
	t_token					*redirection_out;
	struct s_cmd			*next;
}	t_cmd;

///------------------------Defines------------------------///

#define ATOI_ERROR 3000000000
#define MALLOC_ERROR 42

///------------------------Functions------------------------///

//garbage collector
int		add_to_trash(t_garbage_collect **root, void *to_free);
void    *malloc_trash(int size, t_garbage_collect **gc);
int 	empty_trash(t_garbage_collect *gc);
void	*setter_gc(void *data_to_set, t_garbage_collect **gc);
void	**setter_double_p_gc(void **data_to_set, t_garbage_collect **gc);


//BUILT INS
int		unset(t_env_node *env_dup_root, char *env_to_find);
int		export(t_env_node **root, void *variable, t_garbage_collect **gc);
int 	env(t_env_node *env_dup_root, t_garbage_collect *gc);
int 	ft_exit(char **args, t_garbage_collect *gc);
void	sorted_env_print(t_env_node *env_dup_root, t_garbage_collect *gc);
int		pwd(t_garbage_collect **gc);

//UTILS
size_t	len_to_char(char *str, char c);
int	is_char_in_str(char *str, char c);
int	ft_strcmp(const char *s1, const char *s2);
int	pop(t_env_node *env_dup_root, t_env_node *node_to_pop);
int	generate_env_llist(t_env_node **env_dup_root, t_garbage_collect **gc, char **envp);
int	count_nodes(t_env_node *root);
t_env_node *get_env_node(t_env_node *root, char *variable_name);

//errors && exit
void    perror_exit(t_garbage_collect *gc, int exit_code, char *err_msg);
void	empty_trash_exit(t_garbage_collect *gc, int exit_code);
void    ft_error(char *error, t_garbage_collect *gc);

///------------------------Execution------------------------///
char    *expander(t_env_node *env, t_garbage_collect **gc, char *to_expand);


///------------------------Libft------------------------///
char	*get_next_line(int fd);
char	*ft_strdup(const char *src);
size_t	ft_strlen(const char *s);
char	*ft_strnstr(const char *big, const char *little, size_t len);
int		ft_isalpha(int c);
int		ft_isalnum(int c);
int		ft_isdigit(int c);
int		ft_strncmp(char *s1, char *s2, size_t n);
char	**ft_split(char const *s, char c);
void	ft_free_array(void **array);
int		ft_atoi(const char *nptr);
long	ft_safe_atoi(const char *nptr);
int		ft_printf_err(const char *text, ...);

///------------------------Parser/Lexer------------------------///
void	parse(char **input, t_garbage_collect **gc);
t_token	*tokenize(char **input, t_garbage_collect **gc);
void	add_token(t_token **tokenpile, t_token *new_token);
t_token	*dup_token(t_token *token, t_garbage_collect **gc);
void	set_to_last_redir(t_token **tokenpile);
char	**quote_split(char *input, t_garbage_collect **gc);
#endif
