/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/30 18:18:21 by tfreydie         ###   ########.fr       */
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
typedef struct s_cmd_theo
{
	char						**str; //la commande et ses flags/argument
    t_token					    *redirection_in;
	t_token					    *redirection_out;
	struct s_cmd_theo			*next;
	int							cmd_id; //l'int qui va stock la valeur de retour de la cmd
}	my_cmd;

int         count_pipes(my_cmd *cmds);
static void	init_pipes(int **pipes,int pipe_number, t_garbage_collect *gc);
static int	**malloc_pipes_fds(int pipe_number, t_garbage_collect **gc);
int			count_valid_nodes(t_env_node *root);
void		close_all_pipes(int **pipes_fds, t_garbage_collect *gc);
char		*find_env_variable(char **envp, char *env_to_find);
static char	*ft_strjoin_and_add(char const *s1, char const *s2, char c);
void		secure_dup2(int mew_fd, int olc_fd, int **pipes, t_garbage_collect *gc);
void		child_process(char **envp, my_cmd *cmds, t_garbage_collect **gc, int **pipes);
void		print_open_err_msg_exit(int errno, char *file, t_garbage_collect *gc);

//execve a besoin de deux choses, le char ** de la commande, et envp avec un path valide;
int exec(t_env_node *root_env, my_cmd *cmds, t_garbage_collect **gc)
{
    int 	number_of_pipes;
    int 	**pipes_fds;
	char	**envp;

	envp = rebuild_env(root_env, gc);
	//Pipes should be generated outside of exec so I can then
	//populate each cmd with the correct redirection;
	
	//TODO MOVE BELOW
    number_of_pipes = count_pipes(cmds);
    pipes_fds = malloc_pipes_fds(number_of_pipes, gc);
    init_pipes(pipes_fds, number_of_pipes, *gc);
	//TODO move above
    
	my_cmd *current = cmds;
    while (current)
	{
		child_process(envp, current, gc, pipes_fds); //giving current command !!
		current = current->next;
	}
	close_all_pipes(pipes_fds, *gc);//TODO, MOVE this
    current = cmds;
	int	status;
	while (current)
	{
		if (waitpid(current->cmd_id, &status, 0) == -1)
			perror_exit(gc, errno, "Error waiting for process");
	}
	return (WEXITSTATUS(1)); //replace by exit status;
}
//plusieurs moyen de compter le nombre de commande, je peux le faire avec le nombre
// de Pipe token, ou a priori je peux le faire juste en comptant le nombre de nodes commandes
//a voir lequel est le plus pertinent
void	child_process(char **envp, my_cmd *cmds, t_garbage_collect **gc, int **pipes)
{
	char	*valid_path;
	
	cmds->cmd_id = fork();
	if (cmds->cmd_id == -1)
		perror_exit(gc, errno, "Error creating subshell");
	if (cmds->cmd_id == 0)
	{
		process_behavior(cmds, gc, pipes);	
		close_all_pipes(pipes, gc);
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

void	process_behavior(my_cmd *cmds, t_garbage_collect **gc, int **pipes)
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
			if (here_doc(in->next->str, gc, tmp_fd) == 0);
				exit(42); //WILL HAVE TO HANDLE MANY HERE_DOC LATER
		}
		if (in->type == PIPE)
			tmp_fd = out->str; //str[0]somewhere else//get the already open read pipe fd; // GOTTA CHANGE STR
		secure_dup2(tmp_fd, STDIN_FILENO, pipes, *gc);
		if (in->type == LESS || in->type == D_LESS)
			if (close(in->next->str) == -1)
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
			tmp_fd = out->str; //str[1]somewhere else//assuming we change token so str has the pipe end;
		secure_dup2(tmp_fd, STDOUT_FILENO, pipes, *gc);
		if (out->type == GREAT || out->type == D_GREAT)
			if (close(out->next->str) == -1)
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

int count_pipes(my_cmd *cmds)
{
    int pipe_count;
    
    pipe_count = 0;
    while (cmds)
    {
        if (cmds->redirection_out == PIPE)
            pipe_count++;
        cmds = cmds->next;
    }
    return (pipe_count);
}

static void	init_pipes(int **pipes,int pipe_number, t_garbage_collect *gc)
{
	int	i;

	i = 0;
	while (i < pipe_number)
	{
		if (pipe(pipes[i]) == -1)
			perror_exit(gc, errno, "Failed to open pipe");
		i++;
	}
	return ;
}

static int	**malloc_pipes_fds(int pipe_number, t_garbage_collect **gc)
{
	int	i;
    int **pipe_fds;

	i = 0;
	pipe_fds = (int **)malloc_trash(sizeof(int *) * pipe_number, gc);
	while (i < pipe_number)
	{
		pipe_fds[i] = (int *)malloc_trash(sizeof(int) * 2, gc);
		i++;
	}
	return (pipe_fds);
}

char    **rebuild_env(t_env_node *root, t_garbage_collect **gc)
{
    int		number_of_variables;
	char	**envp;
	int		i;	
	
	i = 0;
	number_of_variables = count_valid_nodes(root);
    envp = malloc_trash(sizeof(char *) * number_of_variables + 1, gc);
	while (root)
    {
        if (root->variable)
		{
			envp[i] = setter_gc(ft_strjoin(root->variable_name, root->variable), gc);
			malloc_check(envp[i], *gc);
		}
        root = root->next;
    }
	envp[i] == NULL;
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

void	close_all_pipes(int **pipes_fds, t_garbage_collect *gc)
{
	int	i;

	i = 0;
	while (i < pipes_fds)
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
char	*find_valid_path(my_cmd *cmds, char **envp, t_garbage_collect **gc)
{
	char *path;
	char **possible_paths;
	int i;
	
	if (envp == NULL)
		return (NULL);
	if (access(cmds->str, X_OK) == 0)
		return (cmds->str);
	path = find_env_variable(envp, "PATH=");
	if (path == NULL)
		return (NULL);
	possible_paths = (char **)setter_double_p_gc((void **)ft_split(path, ':'), gc);
	malloc_check(possible_paths, *gc);
	i = -1;
	while(possible_paths[++i])
	{
		path = setter_gc(ft_strjoin_and_add(path, cmds->str[0], '/'), gc);
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
	len_env = ft_strlen(env_to_find);
	i = 0;
	while (envp[i])
	{
		if (ft_strnstr(envp[i], env_to_find, len_env))
			return (envp[i] + len_env); // this is so i just get the content
		i++;
	}
	return (NULL);
}

void	secure_dup2(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc)
{
	if (dup2(new_fd, old_fd) == -1)
	{	
		close_all_pipes(pipes, gc);
		perror_exit(gc, errno, "Error duplicating file descriptor");
	}
	return ;
}

void	print_open_err_msg_exit(int errno, char *file, t_garbage_collect *gc)
{
	if (errno = ENOENT)
		if (ft_printf_err("bash: %s: No such file or directory\n", file) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	if (errno = EACCES)
		if (ft_printf_err("bash: %s: Permission denied\n", file) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	if (errno = EISDIR)
		if (ft_printf_err("bash: %s: Is a directory\n", file) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	if (errno = EMFILE)
		if (ft_printf_err("bash: %s: Too many files opened", file) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	empty_trash_exit(gc, errno);
}

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