/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_parse.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:42:23 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 12:42:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_parse(t_env **env, t_gc **gc, char **cmd, int backup_fds[2])
{
	if (cmd == NULL || cmd[0] == NULL)
		return (1);
	if (ft_strcmp(cmd[0], "unset") == 0)
		return (unset(*env, cmd[1]));
	if (ft_strcmp(cmd[0], "export") == 0)
	{	
		if (cmd[1] == NULL)
			return(sorted_env_print(*env, *gc));
		else
			return(export(env, (void *)cmd[1], gc));
	}
	if (ft_strcmp(cmd[0], "env") == 0)
		return (env_builtin(*env, *gc));
	if (ft_strcmp(cmd[0], "exit") == 0)
		return (ft_exit(&cmd[1], *gc, backup_fds));
	if (ft_strcmp(cmd[0], "pwd") == 0)
		pwd(gc);
	if (ft_strcmp(cmd[0], "cd") == 0)
		return (cd(cmd, gc, *env));
	if (ft_strcmp(cmd[0], "echo") == 0)
		return(echo(cmd, gc));
	return (0);
}