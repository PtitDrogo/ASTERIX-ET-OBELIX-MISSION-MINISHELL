/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:16:38 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 14:59:14 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*accurate_shell(t_gc **gc, t_env *env);

char	*prompt(t_gc **gc, t_env *env)
{
	char	*pwd;

	pwd = accurate_shell(gc, env);
	if (pwd)
	{
		pwd = setter_gc(ft_strjoin("Minishell:", pwd), gc);
		pwd = setter_gc(ft_strjoin(pwd, "$ "), gc);
	}
	else
		pwd = ("Minishell:$ ");
	return (pwd);
}

static char	*accurate_shell(t_gc **gc, t_env *env)
{
	t_env	*pwd;
	char	*backup_pwd;

	pwd = get_env_node(env, "PWD");
	if (pwd != NULL)
		return (pwd->variable);
	else
	{
		backup_pwd = getcwd(NULL, 0);
		setter_gc(backup_pwd, gc);
		return (backup_pwd);
	}
}
