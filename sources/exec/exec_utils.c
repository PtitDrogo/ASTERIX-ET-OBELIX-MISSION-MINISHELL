/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/03 18:52:11 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
// #include "pipex_bonus.h"

//tmp struct for me to play with while not breaking your stuff

//WE populate the redirection before exec;

/*
typedef struct s_token
{
	char			*str;
	t_tok_val		type;
	struct s_token	*next;
}	t_token;
*/
// typedef struct s_cmd_theo
// {
// 	char						**str; //la commande et ses flags/argument
//     t_token					    *redirection_in;
// 	t_token					    *redirection_out;
// 	struct s_cmd_theo			*next;
// 	int							cmd_id; //l'int qui va stock la valeur de retour de la cmd
// }	t_cmd;



int			count_valid_nodes(t_env_node *root);
void		close_all_pipes(int **pipes_fds, t_garbage_collect *gc, int number_of_pipes);
char		*find_env_variable(char **envp, char *env_to_find);
static char	*ft_strjoin_and_add(char const *s1, char const *s2, char c);
void		secure_dup2(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes);
void		print_open_err_msg_exit(int errnumber, char *file, t_garbage_collect *gc);
char		**rebuild_env(t_env_node *root, t_garbage_collect **gc);
void		process_behavior(t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes);
char		*find_valid_path(t_cmd *cmds, char **envp, t_garbage_collect **gc);
void	child_process(char **envp, t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes);

//execve a besoin de deux choses, le char ** de la commande, et envp avec un path valide;
int exec(t_env_node *root_env, t_cmd *cmds, t_garbage_collect **gc, int **pipes_fds, int number_of_pipes)
{

	char	**envp;
	t_cmd *current = cmds;
	int	status;

	envp = rebuild_env(root_env, gc);
	printf("envp after rebuild is %p and first var is %s\n", envp, envp[0]);
    while (current)
	{
		child_process(envp, current, gc, pipes_fds, number_of_pipes); //giving current command !!
		current = current->next;
	}
	// close_all_pipes(pipes_fds, *gc); // WE WILL CLOSE OUTSIDE OF FUNCTION;
    current = cmds;
	printf("in parent, ID of child is %i\n", current->cmd_id);
	while (current)
	{
		if (waitpid(current->cmd_id, &status, 0) == -1)
			perror_exit(*gc, errno, "Error waiting for process");
	}
	return (WEXITSTATUS(1)); //replace by exit status;
}
//plusieurs moyen de compter le nombre de commande, je peux le faire avec le nombre
// de Pipe token, ou a priori je peux le faire juste en comptant le nombre de nodes commandes
//a voir lequel est le plus pertinent
void	child_process(char **envp, t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes)
{
	char	*valid_path;
	
	cmds->cmd_id = fork();
	if (cmds->cmd_id == -1)
		perror_exit(*gc, errno, "Error creating subshell");
	if (cmds->cmd_id == 0)
	{
		process_behavior(cmds, gc, pipes, number_of_pipes);	
		close_all_pipes(pipes, *gc, number_of_pipes);
		//Si j'ouvre des fichiers faut que je me demmerde pour les closes apres;
		
		valid_path = find_valid_path(cmds, envp, gc);
		if (valid_path == NULL)
		{
			ft_printf_err("%s: command not found\n", cmds->str[0]); //need to check real err msg
			empty_trash_exit(*gc, 127);
		}
		execve(valid_path, cmds->str, envp);
		ft_printf_err("Execve failed\n");
		empty_trash_exit(*gc, 127);
	}
}

void	process_behavior(t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes)
{
	//je veux just dup les redirections;
	t_token	*in;
	t_token	*out;
	int		tmp_fd;

	in = cmds->redirection_in;
	out = cmds->redirection_out;
	
	if (in)
	{	
		if (in->type == LESS)
		{	
			tmp_fd = open(in->next->str, O_RDONLY);
			if (tmp_fd == -1)
				print_open_err_msg_exit(errno, in->next->str, *gc);
		}
		if (in->type == D_LESS)
		{
			tmp_fd = open(".ft_heredoc", O_CREAT | O_WRONLY | O_TRUNC, 0777);
			if (tmp_fd == -1)
				print_open_err_msg_exit(errno, in->next->str, *gc);
			if (here_doc(in->next->str, gc, tmp_fd) == 0)
				exit(42); //WILL HAVE TO HANDLE MANY HERE_DOC LATER
		}
		if (in->type == PIPE)
			tmp_fd = out->pipe_fd; //str[0]somewhere else//get the already open read pipe fd; // GOTTA CHANGE STR
		secure_dup2(tmp_fd, STDIN_FILENO, pipes, *gc, number_of_pipes);
		if (in->type == LESS || in->type == D_LESS)
			if (close(tmp_fd) == -1)
				perror_exit(*gc, errno, "Failed to close opened file");
	}
	if (out)
	{
		if (out->type == GREAT)
		{	
			tmp_fd = open(out->next->str, O_WRONLY | O_TRUNC | O_CREAT, 0644);
			if (tmp_fd == -1)
				print_open_err_msg_exit(errno, out->next->str, *gc);
		}
		if (out->type == D_GREAT)
		{	
			tmp_fd = open(out->next->str, O_WRONLY | O_APPEND | O_CREAT, 0644);
			if (tmp_fd == -1)
				print_open_err_msg_exit(errno, out->next->str, *gc);
		}	
		if (out->type == PIPE)
			tmp_fd = out->pipe_fd; //str[1]somewhere else//assuming we change token so str has the pipe end;
		secure_dup2(tmp_fd, STDOUT_FILENO, pipes, *gc, number_of_pipes);
		if (out->type == GREAT || out->type == D_GREAT)
			if (close(tmp_fd) == -1)
				perror_exit(*gc, errno, "Failed to close opened file");
	}
	return ; // if theres no redirection we just go to exec as usual;
}

static char	*ft_strjoin_and_add(char const *s1, char const *s2, char c)
{
	char	*joined;
	size_t	i;
	size_t	j;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	j = 0;
	joined = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 2));
	if (!joined)
		return (NULL);
	while (i < ft_strlen(s1))
	{
		joined[i] = s1[i];
		i++;
	}
	joined[i] = c;
	i++;
	while (j < ft_strlen(s2))
	{
		joined[i++] = s2[j++];
	}
	joined[i] = '\0';
	return (joined);
}

char    **rebuild_env(t_env_node *root, t_garbage_collect **gc)
{
    int		number_of_variables;
	char	**envp;
	int		i;	
	
	i = 0;
	number_of_variables = count_valid_nodes(root);
	printf("number of variable is %i\n", number_of_variables);
    envp = malloc_trash(sizeof(char *) * (number_of_variables + 1), gc);
	while (root)
    {
        if (root->variable)
		{
			envp[i] = setter_gc(ft_strjoin(root->variable_name, root->variable), gc);
			malloc_check(envp[i], *gc);
		}
        root = root->next;
		// printf("current var being set is %s\n", envp[i]);
		i++;
    }
	printf("i is equal to %i (we expect var)\n", i);
	envp[i] = NULL;
	return (envp);
}

int	count_valid_nodes(t_env_node *root)
{
    int valid_nodes;
    
	valid_nodes = 0;
	while (root)
	{
        if (root->variable)
            valid_nodes++;
		root = root->next;
	}
	return (valid_nodes);
}

void	close_all_pipes(int **pipes_fds, t_garbage_collect *gc, int number_of_pipes)
{
	int	i;

	i = 0;
	while (i < number_of_pipes)
	{
		if (close(pipes_fds[i][0]) == -1)
			perror_exit(gc, errno, "Failed to close pipe");
		if (close(pipes_fds[i][1]) == -1)
			perror_exit(gc, errno, "Failed to close pipe");
		i++;
	}
	return ;
}

//J'ai besoin du char de base pour test si c'est un path.
//puis j'ai besoin de l'env reconstruit;
char	*find_valid_path(t_cmd *cmds, char **envp, t_garbage_collect **gc)
{
	char *path;
	char **possible_paths;
	int i;
	
	// printf("in valid path, envp = %p first env is %s\n", envp, envp[0]);
	if (envp == NULL)
		return (NULL);
	if (access(*(cmds->str), X_OK) == 0)
		return (*(cmds->str));
	path = find_env_variable(envp, "PATH");
	// printf("is path null %p\n", path);
	if (path == NULL)
		return (NULL);
	printf("path is %s\n", path);
	possible_paths = (char **)setter_double_p_gc((void **)ft_split(path, ':'), gc);
	malloc_check(possible_paths, *gc);
	i = -1;
	while(possible_paths[++i])
	{
		// printf("possible path is %s\n", possible_paths[i]);
		path = setter_gc(ft_strjoin_and_add(possible_paths[i], cmds->str[0], '/'), gc);
		// printf("testing path = %s\n", path);
		malloc_check(path, *gc);
		if (access(path, X_OK) == 0)
			return (path);
	}
	return (NULL);
}
//tries to find env and return NULL if it doesnt find it;
char	*find_env_variable(char **envp, char *env_to_find)
{
	int	i;
	int	len_env;

	if (!env_to_find || !envp || !envp[0]) // uh ?
		return (NULL);
	printf("hi\n");
	len_env = ft_strlen(env_to_find);
	i = 0;
	while (envp[i])
	{
		
		if (ft_strnstr(envp[i], env_to_find, len_env))
			return (envp[i] + len_env); // this is so i just get the content
		i++;
	}
	printf("double hi\n");
	return (NULL);
}

void	secure_dup2(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes)
{
	if (dup2(new_fd, old_fd) == -1)
	{	
		close_all_pipes(pipes, gc, number_of_pipes);
		perror_exit(gc, errno, "Error duplicating file descriptor");
	}
	return ;
}

void	print_open_err_msg_exit(int errnumber, char *file, t_garbage_collect *gc)
{
	if (errnumber == ENOENT)
		if (ft_printf_err("bash: %s: No such file or directory\n", file) == -1)
			perror_exit(gc, errnumber, WRITE_ERR_MSG);
	if (errnumber == EACCES)
		if (ft_printf_err("bash: %s: Permission denied\n", file) == -1)
			perror_exit(gc, errnumber, WRITE_ERR_MSG);
	if (errnumber == EISDIR)
		if (ft_printf_err("bash: %s: Is a directory\n", file) == -1)
			perror_exit(gc, errnumber, WRITE_ERR_MSG);
	if (errnumber == EMFILE)
		if (ft_printf_err("bash: %s: Too many files opened", file) == -1)
			perror_exit(gc, errnumber, WRITE_ERR_MSG);
	empty_trash_exit(gc, errnumber);
}



// char	*ft_strnstr(const char *big, const char *little, size_t len)
// {
// 	size_t	i;
// 	size_t	j;

// 	j = 0;
// 	if (little[0] == '\0')
// 		return ((char *)big);
// 	while (big[j] != '\0' && j < len)
// 	{
// 		i = 0;
// 		if (big[j] == little[i])
// 		{
// 			while (big[i + j] == little[i] && big[i + j] && (j + i) < len)
// 			{
// 				i++;
// 			}
// 			if (little[i] == '\0')
// 				return ((char *)(big + j));
// 		}
// 		j++;
// 	}
// 	return (NULL);
// }

/*  ENOENT: No such file or directory. The specified file or directory does not exist.
    EACCES: Permission denied. The requested access to the file is not allowed.
    EEXIST: File exists. The file to be created already exists and the O_CREAT and O_EXCL flags were used.
    EISDIR: Is a directory. Attempting to open a directory for writing or reading when the operation requires a regular file.
    EMFILE: Too many open files. The process has reached its limit for open file descriptors.
    ENFILE: File table overflow. The system's file table is full, indicating that the system limit on the total number of open files has been reached.
    EFAULT: Bad address. The pathname argument points outside the accessible address space.*/
//Random syntax check function

//I think this is useless and we should check this before e	xec
// if (in->next == NULL || in->next->type != STR)
// {
// 	ft_printf_err(SYNTAX_ERROR_MSG); //for cases like < | or < <
// 	empty_trash_exit(*gc, SYNTAX_ERROR);
// }