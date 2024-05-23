/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/23 15:03:48 by garivo           ###   ########.fr       */
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
// int		tmp_fd;

// void check_fd(int fd) {
//     if (fcntl(fd, F_GETFD) == -1) {
//         perror("fcntl - GETFD");
//         printf("Error checking FD %d: %s\n", fd, strerror(errno));
//     } else {
//         printf("FD %d is open\n", fd);
//         int flags = fcntl(fd, F_GETFL);
//         if (flags == -1) {
//             perror("fcntl - GETFL");
//         } else {
//             printf("FD %d access mode: %s\n", fd, (flags & O_ACCMODE) == O_RDONLY ? "read-only" :
//                                               (flags & O_ACCMODE) == O_WRONLY ? "write-only" :
//                                               (flags & O_ACCMODE) == O_RDWR   ? "read/write" : "unknown");
//         }
//         int cloexec = fcntl(fd, F_GETFD);
//         if (cloexec == -1) {
//             perror("fcntl - GETFD");
//         } else {
//             printf("FD %d close-on-exec flag is %s\n", fd, (cloexec & FD_CLOEXEC) ? "set" : "not set");
//         }
//     }
// }

// int		tmp_fd;









int			count_valid_nodes(t_env_node *root);
void		close_all_pipes(int **pipes_fds, t_garbage_collect *gc, int number_of_pipes);
char		*find_env_variable(char **envp, char *env_to_find);
static char	*ft_strjoin_and_add(char const *s1, char const *s2, char c);
void		secure_dup2(int new_fd, int old_fd, int **pipes, t_garbage_collect *gc, int number_of_pipes);
void		print_open_err_msg_exit(int errnumber, char *file, t_garbage_collect *gc);
char		**rebuild_env(t_env_node *root, t_garbage_collect **gc);
void		process_behavior(t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes);
char		*find_valid_path(t_cmd *cmds, char **envp, t_garbage_collect **gc);
void		child_process(t_env_node *env, char **envp, t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes);
//ca fait beaucoup la non

//execve a besoin de deux choses, le char ** de la commande, et envp avec un path valide;
int			get_status_code(t_garbage_collect **gc, int status);


int		get_status_code(t_garbage_collect **gc, int status)
{
	//WIFEXITED(status) Check si le process a ete termine par un return ou exit (et non par un signal)
	//si c'est le cas, je renvois status process par la bonne macro qui va avec
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else
		return (130); //Quand je teste c'est un 130 le return dun process ctrl C mais ca peut changer au besoin;
}


int exec(t_env_node *root_env, t_cmd *cmds, t_garbage_collect **gc, int **pipes_fds, int number_of_pipes)
{

	//BUILTIN ARE MAIN PROCESS ALONE
	//CHILDREN PROCESS OTHERWISE
	char	**envp;
	t_cmd *current = cmds;
	int	status;

	envp = rebuild_env(root_env, gc);
	signal(SIGINT, cancel_cmd);
	while (current)
	{
		child_process(root_env, envp, current, gc, pipes_fds, number_of_pipes); //giving current command !!
		current = current->next;
	}
	close_all_pipes(pipes_fds, *gc, number_of_pipes);
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
	status = get_status_code(gc, status);
	return (status); //replace by exit status;
}
//plusieurs moyen de compter le nombre de commande, je peux le faire avec le nombre
// de Pipe token, ou a priori je peux le faire juste en comptant le nombre de nodes commandes
//a voir lequel est le plus pertinent
void	child_process(t_env_node *env, char **envp, t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes)
{
	char	*valid_path;
	
	cmds->cmd_id = fork();
	if (cmds->cmd_id == -1)
		perror_exit(*gc, errno, "Error creating subshell");
	if (cmds->cmd_id == 0)
	{
		process_behavior(cmds, gc, pipes, number_of_pipes);	
		close_all_pipes(pipes, *gc, number_of_pipes);
		valid_path = find_valid_path(cmds, envp, gc);
		if (valid_path == NULL && cmds && cmds->str && is_builtin(cmds->str) == false) //last condition is important !
		{
			ft_printf_err("%s: command not found\n", cmds->str[0]); //need to check real err msg
			empty_trash_exit(*gc, 127);
		}
		else if (cmds && cmds->str)
		{
			printf("hi, cmd is %s\n", cmds->str[0]);
			if (is_builtin(cmds->str))
			{	
				printf("hi, cmd is %s\n", cmds->str[0]);
				theo_basic_parsing(&env, gc, cmds->str);
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

void	process_behavior(t_cmd *cmds, t_garbage_collect **gc, int **pipes, int number_of_pipes)
{
	//je veux just dup les redirections;
	t_token	*in;
	t_token	*out;
	int		tmp_fd;
	int		status;

	in = cmds->redirection_in;
	out = cmds->redirection_out;
	status = 0;
	while (in)
	{	
		if (in->type == LESS)
		{	
			tmp_fd = open(in->next->str, O_RDONLY);
			if (tmp_fd == -1)
				print_open_err_msg_exit(errno, in->next->str, *gc);
		}
		if (in->type == D_LESS)
		{
			tmp_fd = open(HEREDOC_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0777);
			if (tmp_fd == -1)
				print_open_err_msg_exit(errno, in->next->str, *gc);
			status = here_doc(in->next->str, gc, tmp_fd);
			if (status == EXIT_SUCCESS)
			{
				ft_printf("Heredoc success\n");
				signal(SIGINT, cancel_cmd);
				close(tmp_fd);
				tmp_fd = open(HEREDOC_FILE, O_RDONLY);
			}
		}
		if (in->type == PIPE)
			tmp_fd = in->pipe_fd;
		if (in->next && in->next->next == NULL || in->type == PIPE)
			secure_dup2(tmp_fd, STDIN_FILENO, pipes, *gc, number_of_pipes);
		if (in->type == LESS || in->type == D_LESS)
			if (close(tmp_fd) == -1)
				perror_exit(*gc, errno, "Failed to close opened file");
		in = in->next;
	}
	while (out)
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
			tmp_fd = out->pipe_fd;
		if ((out->next && out->next->next == NULL) || out->type == PIPE)
			secure_dup2(tmp_fd, STDOUT_FILENO, pipes, *gc, number_of_pipes);
		if (out->type == GREAT || out->type == D_GREAT)
			if (close(tmp_fd) == -1)
				perror_exit(*gc, errno, "Failed to close opened file");
		out = out->next;
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
	
	if (envp == NULL || cmds == NULL || cmds->str == NULL) // maybe add an error message for some of these cases;
		return (NULL);
	if (access(*(cmds->str), X_OK) == 0)
		return (*(cmds->str));
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
