/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:54:37 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/20 20:00:36 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_error(char *dir_path, t_garbage_collect *gc);
static char	*join_path_to_home(char *dir_path, char *home, t_garbage_collect **gc);
static void	update_pwd(t_garbage_collect **gc, t_env_node *env, char *dir_path);
char	*safe_get_var(t_env_node *env, t_garbage_collect *gc, char *variable);

// Should be all done, work with cd - and with ~/ at the start;
int cd(char **cmd, t_garbage_collect **gc, t_env_node *env)
{   
	char	*home;
	char	*dir_path;

	if (cmd == NULL || cmd[0] == NULL)
		return (0);
	if (count_arrays_in_doubleptr((void **)cmd) > 2)
	{
		if (ft_printf_err("bash: cd: too many arguments\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		return (1); //value of echo $?
	}
	dir_path = cmd[1];
	if (dir_path == NULL)
		dir_path = safe_get_var(env, *gc, "HOME");
	else if (ft_strcmp(cmd[1], "-") == 0)
		dir_path = safe_get_var(env, *gc, "OLDPWD");
	else if (ft_strncmp(dir_path, "~", 1) == 0)	
		dir_path = join_path_to_home(dir_path, safe_get_var(env, *gc, "HOME"), gc);
	if (dir_path == NULL)
		return (1);
	update_pwd(gc, env, dir_path);
	return (0);
}

//safely gets variable content, prints err msg itself and returns null otherwise
char	*safe_get_var(t_env_node *env, t_garbage_collect *gc, char *variable)
{
	char	*var_content;
	
	var_content = get_env_variable(env, variable);
	if (var_content == NULL)
	{
		if (ft_printf_err("bash: cd: %s not set\n", variable) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		return (NULL);
	}
	return (var_content);
}

static void	handle_error(char *dir_path, t_garbage_collect *gc)
{
	if (errno == EACCES)
	{	
		if (ft_printf_err("bash: cd: %s: Permission denied\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else if (errno == ENOENT)
	{	
		if (ft_printf_err("bash: cd: %s: No such file or directory\n",  dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else if (errno == ENOTDIR)
	{	
		if (ft_printf_err("bash: cd: %s: Not a directory\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else if (errno == EFAULT)
	{
		if (ft_printf_err("bash: cd: %s: Path outside of range of process\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else
	{    
		if (ft_printf_err("bash: cd: %s: Couldn't change directory\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
}

char	*join_path_to_home(char *dir_path, char *home, t_garbage_collect **gc)
{
	char *to_return;

	if (home == NULL)
		return (NULL);
	to_return = ft_strjoin(home, &dir_path[1]); //skip past the ~
	if (to_return == NULL)
	{
		if (ft_printf_err(MALLOC_ERR_MSG) == -1)
			perror_exit(*gc, errno, PERROR_ERR_MSG);
		empty_trash_exit(*gc, MALLOC_ERROR);
	}
	setter_gc(to_return, gc);
	return (to_return);
}
//CD AND PWD FAILURE DONT EXIT SHELL
void	update_pwd(t_garbage_collect **gc, t_env_node *env, char *dir_path)
{
	t_env_node *pwd_old;
	t_env_node *pwd_curr;
	char *back_up_old_pwd;

	back_up_old_pwd = setter_gc(getcwd(NULL, 0), gc);
	// if (back_up_old_pwd == NULL)
	// 	perror_exit(*gc, errno, "Failed to get current path");
	if (chdir(dir_path) == -1)
		handle_error(dir_path, *gc);
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
	return ;
}
