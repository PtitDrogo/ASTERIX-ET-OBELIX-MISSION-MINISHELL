/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 22:42:42 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 15:22:29 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_unvalid_path(t_data *data, t_exec *exec);
static void	child_process(t_data *data, t_exec *exec);
static char	*find_valid_path(t_cmd *cmd, char **envp, t_gc **gc);

int	exec(t_data *data, int **pipes_fds, int number_of_pipes)
{
	t_exec	exec;

	ft_memset(&exec, 0, sizeof(exec));
	init_exec(&exec, data, pipes_fds, number_of_pipes);
	while (exec.cmd_cur)
	{
		child_process(data, &exec);
		exec.token_cur = get_next_first_token(exec.token_cur);
		exec.cmd_cur = exec.cmd_cur->next;
	}
	close_all_pipes(pipes_fds, data->gc, number_of_pipes);
	close_all_heredoc_pipes(data->cmds, data->gc);
	exec.cmd_cur = data->cmds;
	while (exec.cmd_cur)
	{
		waitpid(exec.cmd_cur->cmd_id, &exec.status, 0);
		if (handle_status(&exec.status) == -1)
			empty_trash_exit(data->gc, 1);
		exec.cmd_cur = exec.cmd_cur->next;
	}
	//REMOVE ME DURING THE MERGE
	if (exec.status == 130)
	{
		if (ft_printf("\n") == -1)
			return (2);
	}
	if (exec.status == 131)
		if (ft_printf("Core Dumped\n") == -1)
			return (2);
	//REMOVE ME DURING THE MERGE
	return (exec.status);
}

static void	handle_unvalid_path(t_data *data, t_exec *exec)
{
	if (errno == EISDIR)
	{
		ft_printf2("bash: %s: Is a directory\n", exec->cmd_cur->str[0]);
		empty_trash_exit(data->gc, 126);
	}
	else if (errno == EACCES)
	{
		ft_printf2("bash: %s: Permission denied\n", exec->cmd_cur->str[0]);
		empty_trash_exit(data->gc, 126);
	}
	else
	{
		ft_printf2("bash: %s: command not found\n", exec->cmd_cur->str[0]);
		empty_trash_exit(data->gc, 127);
	}
}

static void	child_process(t_data *data, t_exec *exec)
{
	char	*valid_path;
	int		status;

	exec->cmd_cur->cmd_id = fork();
	if (exec->cmd_cur->cmd_id == -1)
		perror_exit(data->gc, errno, "Error creating subshell");
	if (exec->cmd_cur->cmd_id == 0)
	{
		status = process_behavior(exec->cmd_cur, exec->token_cur);
		close_all_pipes(exec->pipes_fds, data->gc, exec->number_of_pipes);
		close_all_heredoc_pipes(data->cmds, data->gc);
		if (status != 0)
			empty_trash_exit(data->gc, 1);
		if (get_correct_cmd(exec->cmd_cur) == 0)
			empty_trash_exit(data->gc, 0);
		valid_path = find_valid_path(exec->cmd_cur, exec->envp, &data->gc);
		handle_directory_case(valid_path, data->gc);
		if (valid_path == NULL && exec->cmd_cur && exec->cmd_cur->str
			&& is_builtin(exec->cmd_cur->str) == false)
			handle_unvalid_path(data, exec);
		else if (exec->cmd_cur && exec->cmd_cur->str)
			handle_command(data, exec, valid_path);
		else
			empty_trash_exit(data->gc, 127);
	}
}

char	**rebuild_env(t_env *root, t_gc **gc)
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
			envp[i] = setter_gc(ft_strjoin_and_add(root->variable_name,
						root->variable, '='), gc);
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

static char	*find_valid_path(t_cmd *cmd, char **envp, t_gc **gc)
{
	char	*path;
	char	**paths;
	int		i;

	if (envp == NULL || cmd == NULL || cmd->str == NULL || cmd->str[0] == NULL)
		return (NULL);
	if (is_char_in_str(*(cmd->str), '/') == true)
		return (try_path(*(cmd->str)));
	path = find_env_variable(envp, "PATH");
	if (path == NULL)
		return (NULL);
	paths = (char **)setter_double_p_gc((void **)ft_split(path, ':'), gc);
	malloc_check(paths, *gc);
	i = -1;
	while (paths[++i])
	{
		path = setter_gc(ft_strjoin_and_add(paths[i], cmd->str[0], '/'), gc);
		malloc_check(path, *gc);
		if (access(path, X_OK) == 0)
			return (path);
	}
	return (NULL);
}
