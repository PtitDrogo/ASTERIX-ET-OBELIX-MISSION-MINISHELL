/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 02:50:57 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	close_all_heredoc_pipes(t_cmd *cmds_root, t_garbage_collect *gc);
t_token	*get_next_first_token(t_token *cmds_root);

void check_fd(int fd) {
    if (fcntl(fd, F_GETFD) == -1) {
        perror("fcntl - GETFD");
        printf("Error checking FD %d: %s\n", fd, strerror(errno));
    } else {
        printf("FD %d is open\n", fd);
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1) {
            perror("fcntl - GETFL");
        } else {
            printf("FD %d access mode: %s\n", fd, (flags & O_ACCMODE) == O_RDONLY ? "read-only" :
                                              (flags & O_ACCMODE) == O_WRONLY ? "write-only" :
                                              (flags & O_ACCMODE) == O_RDWR   ? "read/write" : "unknown");
        }
        int cloexec = fcntl(fd, F_GETFD);
        if (cloexec == -1) {
            perror("fcntl - GETFD");
        } else {
            printf("FD %d close-on-exec flag is %s\n", fd, (cloexec & FD_CLOEXEC) ? "set" : "not set");
        }
    }
}

// int		tmp_fd;

int			count_valid_nodes(t_env_node *root);
void		close_all_pipes(int **pipes_fds, t_garbage_collect *gc, int number_of_pipes);
char		*find_env_variable(char **envp, char *env_to_find);

void		secure_dup2(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes);
int			print_open_err_msg_exit(int errnumber, char *file, t_garbage_collect *gc);

int			process_behavior(t_cmd *cmds, t_garbage_collect **gc, t_token *token_root);
char		*find_valid_path(t_cmd *cmds, char **envp, t_garbage_collect **gc);
void		child_process(t_env_node *env, char **envp, t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes, t_cmd *cmds_root, t_token *token_root);
//ca fait beaucoup la non

//execve a besoin de deux choses, le char ** de la commande, et envp avec un path valide;
int			get_status_code(t_garbage_collect **gc, int status);
int			get_correct_cmd(t_cmd *cmds);


int		get_status_code(t_garbage_collect **gc, int status)
{
	//WIFEXITED(status) Check si le process a ete termine par un return ou exit (et non par un signal)
	//si c'est le cas, je renvois status process par la bonne macro qui va avec
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else
		return (130); //Quand je teste c'est un 130 le return dun process ctrl C mais ca peut changer au besoin;
}


int exec(t_env_node *root_env, t_cmd *cmds, t_garbage_collect **gc, int **pipes_fds, int number_of_pipes, t_cmd *cmds_root, t_token *token_root) //need root to clean pipes;
{

	//BUILTIN ARE MAIN PROCESS ALONE
	//CHILDREN PROCESS OTHERWISE
	char	**envp;
	t_cmd *current = cmds;
	t_token *token_current;
	int	status;

	envp = rebuild_env(root_env, gc);
	token_current = token_root;
	signal(SIGINT, cancel_cmd);
	while (current)
	{
		child_process(root_env, envp, current, gc, pipes_fds, number_of_pipes, cmds_root, token_current); //giving current command !!
		token_current = get_next_first_token(token_current);
		current = current->next;
	}
	close_all_pipes(pipes_fds, *gc, number_of_pipes);
	close_all_heredoc_pipes(cmds_root, *gc);
    current = cmds;
	while (current)
	{
		// printf("in parent, ID of child is %i\n", current->cmd_id);
		waitpid(current->cmd_id, &status, 0);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			status = 128 + WTERMSIG(status);
		if (status == 130)
			ft_printf("\n");
		//perror_exit(*gc, errno, "Error waiting for process");
		current = current->next;
	}
	//status = get_status_code(gc, status);
	return (status); //replace by exit status;
}

t_token *get_next_first_token(t_token *token_root)
{
	//We want to give the token right after the pipe;
	t_token *current;

	current = token_root;
	while (current)
	{
		if (current->type == PIPE)
			return (current);
		current = current->next;
	}
	return (NULL);
}


void	close_all_heredoc_pipes(t_cmd *cmds_root, t_garbage_collect *gc)
{
	t_token *current;

	while (cmds_root)
	{
		current = cmds_root->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
				close(current->here_doc_pipe); //should care about if this close can fail later;
			current = current->next;
		}
		cmds_root = cmds_root->next;
	}
	return ;
}

void	child_process(t_env_node *env, char **envp, t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes, t_cmd *cmds_root, t_token *token_current)
{
	char	*valid_path;
	int process_status; 
	
	cmds->cmd_id = fork();
	if (cmds->cmd_id == -1)
		perror_exit(*gc, errno, "Error creating subshell");
	if (cmds->cmd_id == 0)
	{
		
		process_status = process_behavior(cmds, gc, token_current); //I always exit since im in child;
		//in close all pipes add function to close all Heredoc pipes (need to give the root of cmd to see function);
		close_all_pipes(pipes, *gc, number_of_pipes);
		close_all_heredoc_pipes(cmds_root, *gc);
		if (process_status != 0)
			empty_trash_exit(*gc, 1);
		if (get_correct_cmd(cmds) == 0)
			empty_trash_exit(*gc, 0); //Bash just exits with return 0 for $NOTEXIST;
		valid_path = find_valid_path(cmds, envp, gc);
		// printf("valid path is %s\n", valid_path);
		if (valid_path == NULL && cmds && cmds->str && is_builtin(cmds->str) == false) //last condition is important !
		{
			// printf("sex\n");
			if (errno == EISDIR )
			{
				ft_printf_err("%s: Is a directory\n", cmds->str[0]); //need to check real err msg
				empty_trash_exit(*gc, 126);
			}
			else if (errno == EACCES)
			{
				ft_printf_err("%s: Permission denied\n", cmds->str[0]); //need to check real err msg
				empty_trash_exit(*gc, 126);
			}
			// else if (errno == ENOENT)
			// {
			// 	ft_printf_err("%s: No such file or directory\n", cmds->str[0]); //need to check real err msg
			// 	empty_trash_exit(*gc, 127);
			// }
			else
			{
				ft_printf_err("%s: command not found\n", cmds->str[0]); //need to check real err msg
				empty_trash_exit(*gc, 127);
			}
			
		}
		else if (cmds && cmds->str)
		{
			// printf("hi, cmd is %s\n", cmds->str[0]);
			if (is_builtin(cmds->str))
			{	
				// printf("hi, cmd is %s\n", cmds->str[0]);
				theo_basic_parsing(&env, gc, cmds->str, NULL);
				empty_trash_exit(*gc, 0);  //Exit with success;
			}
			execve(valid_path, cmds->str, envp);
			ft_printf_err("Execve failed\n");
			empty_trash_exit(*gc, 127);
		}
		else
			empty_trash_exit(*gc, 127); //All of this shit purely because of heredoc without a cmd
	}
}
int	get_correct_cmd(t_cmd *cmds)
{
	int i;
	
	i = 0;
	if (cmds && cmds->str)
	{
		while (cmds && cmds->str && cmds->str[i] && cmds->str[i][0] == '\0')
			i++;
		cmds->str = &cmds->str[i];
		if (cmds->str[0] == NULL)
			return (0);
	}
	return (1);
}
//This return 0 on success, 1 on regular error, 2 on CRITICAL error (write failing)
int process_behavior(t_cmd *cmds, t_garbage_collect **gc, t_token *token_current)
{
	//je veux just dup les redirections;
	t_token	*in;
	t_token	*out;
	int		tmp_fd;
	int		status;

	in = cmds->redirection_in;
	out = cmds->redirection_out;
	status = 0;

	//handling very first token (thats always a pipe)
	//honestly this should always trigger so maybe i can raw dog it
	//with tmp_fd = in->pipe_fd; everytime ? Im not sure how bad it would be, gotta try;
	// printf("Dup token is %s\n", token_current->str);
	if (token_current->type == PIPE)
	{
		if (in)
		{
			if (in->type == PIPE)
			{	
				tmp_fd = in->pipe_fd;
				// secure_dup2(tmp_fd, STDIN_FILENO, pipes, *gc, number_of_pipes);
				if (dup2(tmp_fd, STDIN_FILENO) == -1)
					return (1); //one for error DO NOT BOTHER ME ABOUT INCONSISTENCY
			}
			in = in->next;
		}
		token_current = token_current->next;
	}
	
	while (token_current)
	{
		if (token_current->type == D_LESS || token_current->type == LESS)
		{
			if (in)
			{	
				if (in->type == LESS)
				{	
					tmp_fd = open(in->next->str, O_RDONLY);
					if (tmp_fd == -1)
						return (print_open_err_msg_exit(errno, in->next->str, *gc));
					in = in->next;
				}
				if (in->type == D_LESS)
				{	
					tmp_fd = in->here_doc_pipe;
					in = in->next;
				}
				if ((in->next == NULL) || in->type == PIPE)
					if (dup2(tmp_fd, STDIN_FILENO) == -1)
					{	
						if (ft_printf_err("bash: Error Dupplicating file\n") == -1)
							return (2);
						return (1);
					}
				if (in->type == LESS || in->type == D_LESS)
					if (close(tmp_fd) == -1)
					{	
						if (ft_printf_err("Failed to close opened file")== -1)
							return (2);
						return (1); //maybe 2 ?
					}
				in = in->next;
			}	
		}
		else if (token_current->type == D_GREAT || token_current->type == GREAT || token_current->type == PIPE)
		{
			if (out)
			{
				// printf("out is %s\n", out->str);
				if (out->type == GREAT)
				{	
					// printf("yo in great\n");
					tmp_fd = open(out->next->str, O_WRONLY | O_TRUNC | O_CREAT, 0644);
					if (tmp_fd == -1)
						return (print_open_err_msg_exit(errno, out->next->str, *gc));
					out = out->next;
				}
				if (out->type == D_GREAT)
				{	
					// printf("yo in D great\n");
					tmp_fd = open(out->next->str, O_WRONLY | O_APPEND | O_CREAT, 0644);
					if (tmp_fd == -1)
						return (print_open_err_msg_exit(errno, out->next->str, *gc));
					out = out->next;
				}	
				if (out->type == PIPE)
					tmp_fd = out->pipe_fd;
				if ((out->next == NULL) || out->type == PIPE)
					if (dup2(tmp_fd, STDOUT_FILENO) == -1)
					{	
						if (ft_printf_err("bash: Error Dupplicating file\n") == -1)
							return (2);
						return (1);
					}
				if (out->type == GREAT || out->type == D_GREAT)
					if (close(tmp_fd) == -1) //not closing pipe because i am closing all pipes right outside this function
					{	
						if (ft_printf_err("Failed to close opened file")== -1)
							return (2);
						return (1); //maybe 2 ?
					}
				out = out->next;
			}
		}
		if (token_current->type == PIPE)
			break ;
		token_current = token_current->next;
	}
	return (0); // if theres no redirection we just go to exec as usual;
}

char	*ft_strjoin_and_add(char const *s1, char const *s2, char c)
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
//Rebuild env from the list, adding correctly the var if its set;
char    **rebuild_env(t_env_node *root, t_garbage_collect **gc)
{
    int		number_of_variables;
	char	**envp;
	int		i;	
	
	i = 0;
	number_of_variables = count_nodes(root);
    envp = malloc_trash(sizeof(char *) * (number_of_variables + 1), gc);
	while (root)
    {
        if (root->variable)
		{
			envp[i] = setter_gc(ft_strjoin_and_add(root->variable_name, root->variable, '='), gc);
			malloc_check(envp[i], *gc);
		}
		else
		{
			envp[i] = setter_gc(ft_strdup(root->variable_name), gc);
			malloc_check(envp[i], *gc);
		}
        root = root->next;
		i++;
    }
	envp[i] = NULL;
	return (envp);
}
//En fait ca sert a rien les nodes non set sont quand meme dans le env
//je laisse la fonction au cas ou
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
	if (pipes_fds == NULL)
		return ;
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
	
	if (envp == NULL || cmds == NULL || cmds->str == NULL || cmds->str[0] == NULL) //yes we need all of these
		return (NULL);
	if (is_char_in_str(*(cmds->str), '/') == true)
	{
		if (access(*(cmds->str), X_OK) == 0)
		{	
			// printf("nique ta mere\n");
			return (*(cmds->str));
		}
		else
			return (NULL);
	}
	path = find_env_variable(envp, "PATH");
	if (path == NULL)
		return (NULL);
	possible_paths = (char **)setter_double_p_gc((void **)ft_split(path, ':'), gc);
	malloc_check(possible_paths, *gc);
	i = -1;
	while(possible_paths[++i])
	{
		path = setter_gc(ft_strjoin_and_add(possible_paths[i], cmds->str[0], '/'), gc);
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
			return (envp[i] + len_env);
		i++;
	}
	return (NULL);
}

void	secure_dup2(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes)
{
	if (dup2(new_fd, old_fd) == -1)
	{	
		close_all_pipes(pipes, gc, number_of_pipes);
		//ADD HEREDOC PIPES HERE;
		perror_exit(gc, errno, "Error duplicating file descriptor");
	}
	return ;
}
//Prints error message and return 2 it if couldnt do it instead of perror_exit(gc, errnumber, WRITE_ERR_MSG);
int	print_open_err_msg_exit(int errnumber, char *file, t_garbage_collect *gc)
{
	// printf("HIIIIII IM PRINTING ERROR\n");
	if (errnumber == ENOENT)
		if (ft_printf_err("bash: %s: No such file or directory\n", file) == -1)
			return (2);
	if (errnumber == EACCES)
		if (ft_printf_err("bash: %s: Permission denied\n", file) == -1)
			return (2);
	if (errnumber == EISDIR)
		if (ft_printf_err("bash: %s: Is a directory\n", file) == -1)
			return (2);
	if (errnumber == EMFILE)
		if (ft_printf_err("bash: %s: Too many files opened", file) == -1)
			return (2);
	return (1);
	// empty_trash_exit(gc, 1);
}
