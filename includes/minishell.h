/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:14:17 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/13 14:37:45 by garivo           ###   ########.fr       */
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
# include <fcntl.h>
# include <../libft/includes/libft.h>
# include <sys/wait.h>
# include <signal.h>

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

typedef enum e_tok_val //
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
	int				pipe_fd; //maybe fuse this and str later idk;
	t_tok_val		type;
	struct s_token	*next;
	struct s_token	*prev;
}	t_token;

typedef struct s_cmd
{
	char					**str;
	t_token					*redirection_in;
	t_token					*redirection_out;
	struct s_cmd			*next;
	int						cmd_id;
	//int						(*builtin)(t_tools *, struct s_simple_cmds *);
	// int		input;
}	t_cmd;

///------------------------Defines------------------------///

# define ATOI_ERROR 3000000000
# define SYNTAX_ERROR 2
# define MALLOC_ERROR 42
# define MALLOC_ERR_MSG "Error : Malloc failed\n"
# define WRITE_ERR_MSG "Error : Writing failed"
# define SYNTAX_ERROR_MSG "bash: syntax error near unexpected token"
# define PERROR_ERR_MSG "Error : "

extern volatile int	g_input;

///------------------------Functions------------------------///

//--------------------------------debug
void check_fd(int fd);


//garbage collector
int		add_to_trash(t_garbage_collect **root, void *to_free);
void    *malloc_trash(int size, t_garbage_collect **gc);
int 	empty_trash(t_garbage_collect *gc);
void	*setter_gc(void *data_to_set, t_garbage_collect **gc);
void	**setter_double_p_gc(void **data_to_set, t_garbage_collect **gc);
void    malloc_check(void *ptr, t_garbage_collect *gc);

//Here_doc
int		here_doc(char *delimiter, t_garbage_collect **gc, int fd);

//BUILT INS
int		unset(t_env_node *env_dup_root, char *env_to_find);
int		export(t_env_node **root, void *variable, t_garbage_collect **gc);
int 	env(t_env_node *env_dup_root, t_garbage_collect *gc);
int 	ft_exit(char **args, t_garbage_collect *gc);
void	sorted_env_print(t_env_node *env_dup_root, t_garbage_collect *gc);
int		pwd(t_garbage_collect **gc);
int		echo(char **to_echo, t_garbage_collect **gc);
int 	cd(char **cmd, t_garbage_collect **gc, t_env_node *env);

//UTILS
size_t		len_to_char(char *str, char c);
int			is_char_in_str(char *str, char c);
int			ft_strcmp(const char *s1, const char *s2);
int			pop(t_env_node *env_dup_root, t_env_node *node_to_pop);
int			generate_env_llist(t_env_node **env_dup_root, t_garbage_collect **gc, char **envp);
int			count_nodes(t_env_node *root);
t_env_node *get_env_node(t_env_node *root, char *variable_name);
bool		is_builtin(char **cmd);
int			count_arrays_in_doubleptr(void **array);
char		*get_env_variable(t_env_node *root, char *variable_name);


//errors && exit
void    perror_exit(t_garbage_collect *gc, int exit_code, char *err_msg);
void	empty_trash_exit(t_garbage_collect *gc, int exit_code);
void    ft_error(char *error, t_garbage_collect *gc);


///------------------------Execution------------------------///
void	expander(t_env_node *env, t_garbage_collect **gc, t_cmd *cmds);
int		**open_pipes(t_cmd *cmds, t_garbage_collect **gc, int number_of_pipes);
int 	exec(t_env_node *root_env, t_cmd *cmds, t_garbage_collect **gc, int **pipes_fds, int number_of_pipes);
int		count_pipes(t_token *token_list);
int		theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **input);

///------------------------Parser/Lexer------------------------///
int		parse(char **input, t_garbage_collect **gc, t_token	**tokenpile, t_cmd	**cmd_chain);
t_token	*tokenize(char **input, t_garbage_collect **gc);
void	add_token(t_token **tokenpile, t_token *new_token);
t_token	*dup_token(t_token *token, t_garbage_collect **gc);
void	set_to_last_redir(t_token **tokenpile);
char	**quote_split(char *input, t_garbage_collect **gc);
int    syntax_error(t_token *token, t_garbage_collect *gc);

///------------------------Signal handling------------------------///
void	set_signal(void);

#endif
