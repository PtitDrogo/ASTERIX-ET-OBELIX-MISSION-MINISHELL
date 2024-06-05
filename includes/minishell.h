/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:14:17 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 15:05:47 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define  MINISHELL_H

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

typedef struct s_env_node 
{

	struct s_env_node	*next;
	char				*variable_name;
	char				*variable;
}	t_env;

typedef struct s_garbage_collect
{
	void						*to_free;
	struct s_garbage_collect	*next;
} t_gc;

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
	int				token_fd; //maybe fuse this and str later idk;
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

typedef struct s_data
{
	t_env				*env_dup_root;
	t_gc				*gc;
	t_token				*token;
	t_cmd				*cmds;
	char				*input;
	char				*history;
	int					**pipes;
	int					status;
	char				*str_status;


} t_data;

typedef struct s_expand
{
	char	*array;  
	char	*expanded_var; 
	char	*error_value; 
	char	quote;
	int		total_size;
	int		size; 
	int		mode;
	t_env	*env;
	
} t_expand;

typedef struct s_exec
{
	char	**envp;
	t_cmd	*cmd_cur;
	t_token	*token_cur;
	int		status;
	int 	**pipes_fds;
	int 	number_of_pipes;

} t_exec;

///------------------------Defines------------------------///

#define ATOI_ERROR 3000000000
#define	SYNTAX_ERROR 2
#define MALLOC_ERROR 1
#define MALLOC_ERR_MSG "Error : Malloc failed\n"
#define WRITE_ERR_MSG "Error : Writing failed"
#define	SYNTAX_ERROR_MSG "bash: syntax error near unexpected token"
#define PERROR_ERR_MSG "Error : "
#define HEREDOC_FILE ".ft_heredoc"
#define WRITE_ERROR 1

#define STD_EX 1
#define REMOVESQUOTES 2
#define EXPAND 3

///------------------------Functions------------------------///

//--------------------------------debug
void check_fd(int fd);


//garbage collector
int		add_to_trash(t_gc **root, void *to_free);
void    *malloc_trash(int size, t_gc **gc);
int 	empty_trash(t_gc *gc);
void	*setter_gc(void *data_to_set, t_gc **gc);
void	**setter_double_p_gc(void **data_to_set, t_gc **gc);
void    malloc_check(void *ptr, t_gc *gc);
void	recycle_trash_new(t_gc **gc, t_env    *env_dup_root);
int		no_dupplicate_check(void	*data, t_gc *gc);

//Here_doc
int					here_doc(char *delimiter, t_gc **gc, int fd, bool do_expand, t_env *env, char *error_value);
t_gc				**global_gc(t_gc **gc);
int					global_fd(int fd);
int					parse_all_here_docs(t_cmd *cmds, t_gc **gc, t_env *env, char *error_value);
t_gc				**global_gc(t_gc **gc);
t_cmd				*global_cmd(t_cmd *cmds);
int					global_fd(int fd);

//EXPANDER
// char 	**expand(t_env_node *env, t_gc **gc, char **arrays, char *error_value, int mode);
char 	*expand_single_str(t_env *env, t_gc **gc, char *array, char *error_value, int mode);
void	expander(t_env *env, t_gc **gc, t_cmd *cmds, char *error_value);

//expander utils
void	var_up(int *var_1, int *var_2, int add_to_1, int add_to_2);
int		up_quote(char c, char *quote);
int		chars_to_expand(char *str);
char	*get_expand_str(char *str, t_gc **gc);
bool	can_expand(char *quote);
void	fill_string(t_expand *expdr, char *tmp);
int		check_quotes(t_expand *expdr, int *i);
int		tmp_check(t_expand *expdr, int *i, char *tmp);
int		handle_question_mark(t_expand *expdr, int *i, char **tmp);
void	init_expander_struct(t_expand *expdr, char *array, char *error_value, int mode);

//BUILT INS
int		unset(t_env *env_dup_root, char *env_to_find);
int		export(t_env **root, void *variable, t_gc **gc);
int 	env(t_env *env_dup_root, t_gc *gc);
int 	ft_exit(char **args, t_gc *gc, int backup_fds[2]);
int		sorted_env_print(t_env *env_dup_root, t_gc *gc);
int		pwd(t_gc **gc);
int		echo(char **to_echo, t_gc **gc);
int 	cd(char **cmd, t_gc **gc, t_env *env);

//UTILS
size_t		len_to_char(char *str, char c);
int			is_char_in_str(char *str, char c);
int			ft_strcmp(const char *s1, const char *s2);
int			pop(t_env *env_dup_root, t_env *node_to_pop);
int			generate_env_llist(t_env **env_dup_root, t_gc **gc, char **envp);
int			count_nodes(t_env *root);
t_env *get_env_node(t_env *root, char *variable_name);
bool		is_builtin(char **cmd);
int			count_arrays_in_doubleptr(void **array);
char		*env_var(t_env *root, char *variable_name);
char		**rebuild_env(t_env *root, t_gc **gc);
char		*ft_strjoin_and_add(char const *s1, char const *s2, char c);
char		*ft_strncat(char *src, char *dst, int len);



//errors && exit
void    perror_exit(t_gc *gc, int exit_code, char *err_msg);
void	empty_trash_exit(t_gc *gc, int exit_code);
void    ft_error(char *error, t_gc *gc);
int		exit_status(int status);
void	exit_heredoc(int status);
void	free_heredoc(void);
int		print_open_err_msg(int errnumber, char *file);


///------------------------Execution------------------------///

int		**open_pipes(t_cmd *cmds, t_gc **gc, int number_of_pipes);
int 	exec(t_data *data, int **pipes_fds, int number_of_pipes);
int		count_pipes(t_token *token_list);
int		builtin_parse(t_env **env_dup_root, t_gc **gc, char **cmd, int backup_fds[2]);
int		process_behavior(t_cmd *cmds, t_gc **gc, t_token *token_cur);
t_token *get_next_first_token(t_token *token_root);
char	*ft_strjoin_and_add(char const *s1, char const *s2, char c);
void	close_all_pipes(int **pipes_fds, t_gc *gc, int number_of_pipes);
void    close_all_heredoc_pipes(t_cmd *cmds_root, t_gc *gc);
void	init_exec(t_exec *exec, t_data *data, int **pipes_fds, int number_pipes);
int		handle_status(int *status);
void	handle_command(t_data *data, t_exec *exec, char *valid_path);
int		get_correct_cmd(t_cmd *cmds);
char	*find_env_variable(char **envp, char *env_to_find);
char    **rebuild_env(t_env *root, t_gc **gc);
char	*try_path(char *path);



///------------------------Parser/Lexer------------------------///
int		parse(char **input, t_gc **gc, t_token	**tokenpile, t_cmd	**cmd_chain);
t_token	*tokenize(char **input, t_gc **gc);
void	add_token(t_token **tokenpile, t_token *new_token);
t_token	*dup_token(t_token *token, t_gc **gc);
void	set_to_last_redir(t_token **tokenpile);
char	**quote_split(char *input, t_gc **gc);
int    syntax_error(t_token *token, t_gc *gc);

///------------------------Signal handling------------------------///
void	new_prompt(int none);
void	cancel_cmd(int none);
void	cancel_heredoc(int none);
// void	cancel_cmd_coredumped(int none);

#endif
