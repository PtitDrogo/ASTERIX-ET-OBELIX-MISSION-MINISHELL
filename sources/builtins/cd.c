/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:54:37 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 16:19:22 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_path_to_home(char *dir_path, char *home, t_gc **gc);
static int	update_pwd(t_gc **gc, t_env *env, char *dir_path);
char		*safe_get_var(t_env *env, t_gc *gc, char *variable);

int	cd(char **cmd, t_gc **gc, t_env *env)
{
	char	*home;
	char	*dir_path;

	if (cmd == NULL || cmd[0] == NULL)
		return (0);
	if (count_arrays_in_doubleptr((void **)cmd) > 2)
	{
		if (ft_printf2("bash: cd: too many arguments\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		return (1);
	}
	dir_path = cmd[1];
	if (dir_path == NULL)
		dir_path = safe_get_var(env, *gc, "HOME");
	else if (ft_strcmp(cmd[1], "-") == 0)
		dir_path = safe_get_var(env, *gc, "OLDPWD");
	else if (ft_strncmp(dir_path, "~", 1) == 0)
		dir_path = join_path_to_home(dir_path,
				safe_get_var(env, *gc, "HOME"), gc);
	if (dir_path == NULL)
		return (1);
	return (update_pwd(gc, env, dir_path));
	return (0);
}

char	*safe_get_var(t_env *env, t_gc *gc, char *variable)
{
	char	*var_content;

	var_content = env_var(env, variable);
	if (var_content == NULL)
	{
		if (ft_printf2("bash: cd: %s not set\n", variable) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (NULL);
	}
	return (var_content);
}

char	*join_path_to_home(char *dir_path, char *home, t_gc **gc)
{
	char	*to_return;

	if (home == NULL)
		return (NULL);
	to_return = ft_strjoin(home, &dir_path[1]);
	if (to_return == NULL)
	{
		if (ft_printf2(MALLOC_ERR_MSG) == -1)
			perror_exit(*gc, errno, PERROR_ERR_MSG);
		empty_trash_exit(*gc, MALLOC_ERROR);
	}
	setter_gc(to_return, gc);
	return (to_return);
}

//fix this later;
int	update_pwd(t_gc **gc, t_env *env, char *dir_path)
{
	t_env	*pwd_old;
	t_env	*pwd_curr;
	char	*back_up_old_pwd;

	back_up_old_pwd = setter_gc(getcwd(NULL, 0), gc);
	if (chdir(dir_path) == -1)
		return (handle_error(dir_path, *gc), 1);
	pwd_curr = get_env_node(env, "PWD");
	pwd_old = get_env_node(env, "OLDPWD");
	if (pwd_old && pwd_curr)
		pwd_old->variable = pwd_curr->variable;
	if (pwd_curr)
	{
		pwd_curr->variable = setter_gc(getcwd(NULL, 0), gc);
		if (pwd_curr->variable == NULL)
			perror_exit(*gc, errno, "Failed to get current path");
	}
	if (pwd_old && pwd_curr == NULL)
		pwd_old->variable = back_up_old_pwd;
	return (0);
}
