/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:28:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		close_all_heredoc_pipes(t_cmd *cmds_root, t_gc *gc);
t_token		*get_next_first_token(t_token *cmds_root);
void		close_all_pipes(int **pipes_fds, t_gc *gc, int number_of_pipes);
char		*find_env_variable(char **envp, char *env_to_find);
int			process_behavior(t_cmd *cmds, t_gc **gc, t_token *token_root);
char		*find_valid_path(t_cmd *cmds, char **envp, t_gc **gc);
void		child_process(t_env_node *env, char **envp, t_cmd *cmds, t_gc **gc, int **pipes, int number_of_pipes, t_cmd *cmds_root, t_token *token_root);
int			get_correct_cmd(t_cmd *cmds);
int			handle_status(int *status);


int exec(t_env_node *root_env, t_cmd *cmds, t_gc **gc, int **pipes_fds, int number_of_pipes, t_token *token_root) //need root to clean pipes;
{
	char	**envp;
	t_cmd	*current;
	t_token	*token_current;
	int		status;

	current = cmds;
	envp = rebuild_env(root_env, gc);
	token_current = token_root;
	while (current)
	{
		child_process(root_env, envp, current, gc, pipes_fds, number_of_pipes, cmds, token_current); //giving current command !!
		token_current = get_next_first_token(token_current);
		current = current->next;
	}
	close_all_pipes(pipes_fds, *gc, number_of_pipes);
	close_all_heredoc_pipes(cmds, *gc);
    current = cmds;
	while (current)
	{
		waitpid(current->cmd_id, &status, 0);
		if (handle_status(&status) == -1)
			empty_trash_exit(*gc, 1);
		current = current->next;
	}
	return (status);
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

void	child_process(t_env_node *env, char **envp, t_cmd *cmds, t_gc **gc, int **pipes, int number_of_pipes, t_cmd *cmds_root, t_token *token_current)
{
	char	*valid_path;
	int process_status; 
	
	cmds->cmd_id = fork();
	if (cmds->cmd_id == -1)
		perror_exit(*gc, errno, "Error creating subshell");
	if (cmds->cmd_id == 0)
	{
		
		process_status = process_behavior(cmds, gc, token_current);
		close_all_pipes(pipes, *gc, number_of_pipes);
		close_all_heredoc_pipes(cmds_root, *gc);
		if (process_status != 0)
			empty_trash_exit(*gc, 1);
		if (get_correct_cmd(cmds) == 0)
			empty_trash_exit(*gc, 0);
		valid_path = find_valid_path(cmds, envp, gc);
		if (valid_path == NULL && cmds && cmds->str && is_builtin(cmds->str) == false) //last condition is important !
		{
			if (errno == EISDIR )
			{
				ft_printf2("%s: Is a directory\n", cmds->str[0]); //need to check real err msg
				empty_trash_exit(*gc, 126);
			}
			else if (errno == EACCES)
			{
				ft_printf2("%s: Permission denied\n", cmds->str[0]); //need to check real err msg
				empty_trash_exit(*gc, 126);
			}
			// else if (errno == ENOENT)
			// {
			// 	ft_printf2("%s: No such file or directory\n", cmds->str[0]); //need to check real err msg
			// 	empty_trash_exit(*gc, 127);
			// }
			else
			{
				ft_printf2("%s: command not found\n", cmds->str[0]); //need to check real err msg
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
			ft_printf2("Execve failed\n");
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
char    **rebuild_env(t_env_node *root, t_gc **gc)
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
