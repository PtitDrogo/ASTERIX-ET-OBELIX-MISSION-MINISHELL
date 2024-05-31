/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:14:17 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 23:47:17 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL.H
# define  MINISHELL.H

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
# include <sys/types.h>
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
	int				here_doc_pipe;
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
}	t_cmd;

///------------------------Defines------------------------///

#define ATOI_ERROR 3000000000
#define	SYNTAX_ERROR 2
#define MALLOC_ERROR 42
#define MALLOC_ERR_MSG "Error : Malloc failed\n"
#define WRITE_ERR_MSG "Error : Writing failed"
#define	SYNTAX_ERROR_MSG "bash: syntax error near unexpected token"
#define PERROR_ERR_MSG "Error : "
#define HEREDOC_FILE ".ft_heredoc"
#define WRITE_ERROR 666

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
int					here_doc(char *delimiter, t_garbage_collect **gc, int fd, bool do_expand, t_env_node *env, char *error_value);
t_garbage_collect	**global_gc(t_garbage_collect **gc);
int					global_fd(int fd);
int					parse_all_here_docs(t_cmd *cmds, t_garbage_collect **gc, t_env_node *env, char *error_value);
t_garbage_collect	**global_gc(t_garbage_collect **gc);
t_cmd				*global_cmd(t_cmd *cmds);
int					global_fd(int fd);

//EXPANDER
char *remove_quotes(t_garbage_collect **gc, char *array);
char 	**expand(t_env_node *env, t_garbage_collect **gc, char **arrays, char *error_value);
int	count_size_no_quotes(char *array, t_garbage_collect **gc);
char *expand_here_doc_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value);

//BUILT INS
int		unset(t_env_node *env_dup_root, char *env_to_find);
int		export(t_env_node **root, void *variable, t_garbage_collect **gc);
int 	env(t_env_node *env_dup_root, t_garbage_collect *gc);
int 	ft_exit(char **args, t_garbage_collect *gc, int backup_fds[2]);
int		sorted_env_print(t_env_node *env_dup_root, t_garbage_collect *gc);
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
char		**rebuild_env(t_env_node *root, t_garbage_collect **gc);
char		*ft_strjoin_and_add(char const *s1, char const *s2, char c);



//errors && exit
void    perror_exit(t_garbage_collect *gc, int exit_code, char *err_msg);
void	empty_trash_exit(t_garbage_collect *gc, int exit_code);
void    ft_error(char *error, t_garbage_collect *gc);
int		exit_status(int status);
void	exit_heredoc(int status);
void	free_heredoc(void);
int		print_open_err_msg(int errnumber, char *file);


///------------------------Execution------------------------///
void	expander(t_env_node *env, t_garbage_collect **gc, t_cmd *cmds, char *error_value);
int		**open_pipes(t_cmd *cmds, t_garbage_collect **gc, int number_of_pipes);
int 	exec(t_env_node *root_env, t_cmd *cmds, t_garbage_collect **gc, int **pipes_fds, int number_of_pipes, t_cmd *cmds_root, t_token *token_root);
int		count_pipes(t_token *token_list);
int		theo_basic_parsing(t_env_node **env_dup_root, t_garbage_collect **gc, char **cmd, int backup_fds[2]);
int			process_behavior(t_cmd *cmds, t_garbage_collect **gc, t_token *token_current);

void    close_all_heredoc_pipes(t_cmd *cmds_root, t_garbage_collect *gc);

///------------------------Parser/Lexer------------------------///
int		parse(char **input, t_garbage_collect **gc, t_token	**tokenpile, t_cmd	**cmd_chain);
t_token	*tokenize(char **input, t_garbage_collect **gc);
void	add_token(t_token **tokenpile, t_token *new_token);
t_token	*dup_token(t_token *token, t_garbage_collect **gc);
void	set_to_last_redir(t_token **tokenpile);
char	**quote_split(char *input, t_garbage_collect **gc);
int    syntax_error(t_token *token, t_garbage_collect *gc);

///------------------------Signal handling------------------------///
void	new_prompt(int none);
void	cancel_cmd(int none);
void	cancel_heredoc(int none);

#endif
