/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 13:57:34 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		close_all_heredoc_pipes(t_cmd *cmds_root, t_gc *gc);
t_token		*get_next_first_token(t_token *cmds_root);
void		close_all_pipes(int **pipes_fds, t_gc *gc, int number_of_pipes);
char		*find_env_variable(char **envp, char *env_to_find);
int			process_behavior(t_cmd *cmds, t_gc **gc, t_token *token_root);
char		*find_valid_path(t_cmd *cmds, char **envp, t_gc **gc);
void		child_process(t_data *data, t_exec *exec);
int			get_correct_cmd(t_cmd *cmds);
int			handle_status(int *status);


void	init_exec(t_exec *exec, t_data *data, int **pipes_fds, int number_of_pipes)
{
	exec->current_cmd = data->cmds;
	exec->envp = rebuild_env(data->env_dup_root, &data->gc); //this need to close fd if ail
	exec->token_current = data->token;
	exec->pipes_fds = pipes_fds;
	exec->number_of_pipes = number_of_pipes;
}

int exec(t_data *data, int **pipes_fds, int number_of_pipes)
{
	t_exec	exec;
	
	ft_memset(&exec, 0, sizeof(exec));
	init_exec(&exec, data, pipes_fds, number_of_pipes);
	while (exec.current_cmd)
	{
		child_process(data, &exec); //giving current command !!
		exec.token_current = get_next_first_token(exec.token_current);
		exec.current_cmd = exec.current_cmd->next;
	}
	close_all_pipes(pipes_fds, data->gc, number_of_pipes);
	close_all_heredoc_pipes(data->cmds, data->gc);
    exec.current_cmd = data->cmds;
	while (exec.current_cmd)
	{
		waitpid(exec.current_cmd->cmd_id, &exec.status, 0);
		if (handle_status(&exec.status) == -1)
			empty_trash_exit(data->gc, 1);
		exec.current_cmd = exec.current_cmd->next;
	}
	return (exec.status);
}

int		handle_status(int *status)
{
	if (WIFEXITED(*status))
		*status = WEXITSTATUS(*status);
	else if (WIFSIGNALED(*status))
		*status = 128 + WTERMSIG(*status);
	if (*status == 130)
		if (ft_printf("\n") == -1)
			return (2);
	return (1);
}

t_token *get_next_first_token(t_token *token_root)
{
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

void    close_all_heredoc_pipes(t_cmd *cmds_root, t_gc *gc)
{
    t_token *current;

    while (cmds_root)
    {
        current = cmds_root->redirection_in;
        while (current)
        {
            if (current->type == D_LESS)
			{
				if (current->token_fd != -1)
                	if (close(current->token_fd) == -1)
						perror_exit(gc, EXIT_FAILURE, "Failed to close pipe"); //should care about if this close can fail later;
			}
            current = current->next;
        }
        cmds_root = cmds_root->next;
    }
    return ;
}

void	child_process(t_data *data, t_exec *exec)
{
	char	*valid_path;
	int 	process_status; 
	
	exec->current_cmd->cmd_id = fork();
	if (exec->current_cmd->cmd_id == -1)
		perror_exit(data->gc, errno, "Error creating subshell");
	if (exec->current_cmd->cmd_id == 0)
	{
		
		process_status = process_behavior(exec->current_cmd, &data->gc, exec->token_current);
		close_all_pipes(exec->pipes_fds, data->gc, exec->number_of_pipes);
		close_all_heredoc_pipes(data->cmds, data->gc);
		if (process_status != 0)
			empty_trash_exit(data->gc, 1);
		if (get_correct_cmd(exec->current_cmd) == 0)
			empty_trash_exit(data->gc, 0);
		valid_path = find_valid_path(exec->current_cmd, exec->envp, &data->gc);
		if (valid_path == NULL && exec->current_cmd && exec->current_cmd->str && is_builtin(exec->current_cmd->str) == false) //last condition is important !
		{
			if (errno == EISDIR )
			{
				ft_printf2("%s: Is a directory\n", exec->current_cmd->str[0]); //need to check real err msg
				empty_trash_exit(data->gc, 126);
			}
			else if (errno == EACCES)
			{
				ft_printf2("%s: Permission denied\n", exec->current_cmd->str[0]); //need to check real err msg
				empty_trash_exit(data->gc, 126);
			}
			// else if (errno == ENOENT)
			// {
			// 	ft_printf2("%s: No such file or directory\n", exec->current_cmd->str[0]); //need to check real err msg
			// 	empty_trash_exit(data->gc, 127);
			// }
			else
			{
				ft_printf2("%s: command not found\n", exec->current_cmd->str[0]); //need to check real err msg
				empty_trash_exit(data->gc, 127);
			}
			
		}
		else if (exec->current_cmd && exec->current_cmd->str)
		{
			// printf("hi, cmd is %s\n", exec->current_cmd->str[0]);
			if (is_builtin(exec->current_cmd->str))
			{	
				// printf("hi, cmd is %s\n", exec->current_cmd->str[0]);
				theo_basic_parsing(&data->env_dup_root, &data->gc, exec->current_cmd->str, NULL);
				empty_trash_exit(data->gc, 0);  //Exit with success;
			}
			execve(valid_path, exec->current_cmd->str, exec->envp);
			ft_printf2("Execve failed\n");
			empty_trash_exit(data->gc, 127);
		}
		else
			empty_trash_exit(data->gc, 127); //All of this shit purely because of heredoc without a cmd
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

char	*ft_strjoin_and_add(char const *s1, char const *s2, char c)
{
	char	*joined;
	size_t	i;
	size_t	j;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	j = 0;
	joined = malloc(sizeof(char) * (ft_len(s1) + ft_len(s2) + 2));
	if (!joined)
		return (NULL);
	while (i < ft_len(s1))
	{
		joined[i] = s1[i];
		i++;
	}
	joined[i] = c;
	i++;
	while (j < ft_len(s2))
	{
		joined[i++] = s2[j++];
	}
	joined[i] = '\0';
	return (joined);
}
//Rebuild env from the list, adding correctly the var if its set;
char    **rebuild_env(t_env *root, t_gc **gc)
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

void	close_all_pipes(int **pipes_fds, t_gc *gc, int number_of_pipes)
{
	int	i;

	i = 0;
	if (pipes_fds == NULL)
		return ;
	while (i < number_of_pipes)
	{
		if (close(pipes_fds[i][0]) == -1)
			perror_exit(gc, EXIT_FAILURE, "Failed to close pipe");
		if (close(pipes_fds[i][1]) == -1)
			perror_exit(gc, EXIT_FAILURE, "Failed to close pipe");
		i++;
	}
	return ;
}

char	*find_valid_path(t_cmd *cmds, char **envp, t_gc **gc)
{
	char *path;
	char **possible_paths;
	int i;
	
	if (envp == NULL || cmds == NULL || cmds->str == NULL || cmds->str[0] == NULL) //yes we need all of these
		return (NULL);
	if (is_char_in_str(*(cmds->str), '/') == true)
	{
		if (access(*(cmds->str), X_OK) == 0)
			return (*(cmds->str));
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

char	*find_env_variable(char **envp, char *env_to_find)
{
	int	i;
	int	len_env;

	if (!env_to_find || !envp || !envp[0]) // uh ?
		return (NULL);
	len_env = ft_len(env_to_find);
	i = 0;
	while (envp[i])
	{
		
		if (ft_strnstr(envp[i], env_to_find, len_env))
			return (envp[i] + len_env);
		i++;
	}
	return (NULL);
}
