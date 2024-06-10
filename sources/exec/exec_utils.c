/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 14:07:59 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/07 20:02:58 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_exec(t_exec *exec, t_data *data, int **pipes_fds, int number_pipes)
{
	exec->cmd_cur = data->cmds;
	exec->envp = rebuild_env(data->env, &data->gc);
	exec->token_cur = data->token;
	exec->pipes_fds = pipes_fds;
	exec->number_of_pipes = number_pipes;
}

int	handle_status(int *status)
{
	if (WIFEXITED(*status))
		*status = WEXITSTATUS(*status);
	else if (WIFSIGNALED(*status))
		*status = 128 + WTERMSIG(*status);
	if (*status == 130)
	{
		if (ft_printf("\n") == -1)
			return (2);
	}
	else if (*status == 131)
	{
		if (ft_printf("Core Dumped\n") == -1)
			return (2);
	}
	return (1);
}

void	handle_command(t_data *data, t_exec *exec, char *valid_path)
{
	// bool	is_valid;

	// is_valid = (exec && exec->cmd_cur && exec->cmd_cur->str);
	if (is_builtin(exec->cmd_cur->str))
	{
		builtin_parse(&data->env, &data->gc, exec->cmd_cur->str, NULL);
		empty_trash_exit(data->gc, 0);
	}
	// if (data->is_last_cmd == false && is_valid
	// 	&& ft_strnstr(exec->cmd_cur->str[0], "/minishell",
	// 		ft_len(exec->cmd_cur->str[0])))
	// {
	// 	empty_trash_exit(data->gc, 127);
	// }
	execve(valid_path, exec->cmd_cur->str, exec->envp);
	ft_printf2("Error executing command\n");
	empty_trash_exit(data->gc, 127);
}

int	get_correct_cmd(t_cmd *cmds)
{
	int	i;

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

char	*find_env_variable(char **envp, char *env_to_find)
{
	int	i;
	int	len_env;

	if (!env_to_find || !envp || !envp[0])
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
