/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:42:23 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/12 14:12:57 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_parse(t_env **env, t_data *data, char **cmd, int backup_fds[2])
{
	if (cmd == NULL || cmd[0] == NULL)
		return (1);
	if (ft_strcmp(cmd[0], "unset") == 0)
		return (unset(*env, cmd[1]));
	if (ft_strcmp(cmd[0], "export") == 0)
	{
		if (cmd[1] == NULL)
			return (sorted_env_print(*env, data->gc));
		else
			return (export(env, (void *)cmd[1], &data->gc));
	}
	if (ft_strcmp(cmd[0], "env") == 0)
		return (env_builtin(*env, data->gc));
	if (ft_strcmp(cmd[0], "exit") == 0)
		return (ft_exit(&cmd[1], data->gc, backup_fds, data->is_solo_b_in));
	if (ft_strcmp(cmd[0], "pwd") == 0)
		pwd(&data->gc);
	if (ft_strcmp(cmd[0], "cd") == 0)
		return (cd(cmd, &data->gc, *env));
	if (ft_strcmp(cmd[0], "echo") == 0)
		return (echo(cmd, &data->gc));
	return (0);
}
