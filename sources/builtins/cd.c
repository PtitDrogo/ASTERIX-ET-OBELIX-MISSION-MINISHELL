/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:54:37 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/24 20:04:26 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_error(char *dir_path, t_garbage_collect *gc);
static char	*get_home(t_env_node *root);
static char	*join_path_to_home(char *dir_path, char *home, t_garbage_collect **gc);
static void	update_pwd(t_garbage_collect **gc, t_env_node *env);
//need to update PWD and OLDPWD


// Im assuming dir_path will be NULL if no valid arg for cd is given;
int cd(char *dir_path, t_garbage_collect **gc, t_env_node *env)
{   
	char *home;
	
	home = get_home(env);
	if (dir_path == NULL)
		dir_path = home;
	if (ft_strncmp(dir_path, "~", 1) == 0)
		dir_path = join_path_to_home(dir_path, home, gc);
	if (chdir(dir_path) == -1)
		handle_error(dir_path, *gc);
	update_pwd(gc, env);
	return (0);
}
static void	handle_error(char *dir_path, t_garbage_collect *gc)
{
	if (errno == EACCES)
	{	
		if (ft_printf_err("bash: cd: %s: Permission denied\n", dir_path) == -1)
			perror_exit(gc, errno, "Failed to write error message");
	}
	else if (errno == ENOENT)
	{	
		if (ft_printf_err("bash: cd: %s: No such file or directory\n",  dir_path) == -1)
			perror_exit(gc, errno, "Failed to write error message");
	}
	else if (errno == ENOTDIR)
	{	
		if (ft_printf_err("bash: cd: %s: Not a directory\n", dir_path) == -1)
			perror_exit(gc, errno, "Failed to write error message");
	}
	else if (errno == EFAULT)
	{
		if (ft_printf_err("bash: cd: %s: Path outside of range of process\n", dir_path) == -1)
			perror_exit(gc, errno, "Failed to write error message");
	}
	else
	{    
		if (ft_printf_err("bash: cd: %s: Couldn't change directory\n", dir_path) == -1)
			perror_exit(gc, errno, "Failed to write error message");
	}
}

static char	*get_home(t_env_node *root)
{
	t_env_node *home_node;
	
	home_node = get_env_node(root, "HOME");
	if (home_node == NULL)
		return (NULL);
	return (home_node->variable);
}

char	*join_path_to_home(char *dir_path, char *home, t_garbage_collect **gc)
{
	char *to_return;

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

void	update_pwd(t_garbage_collect **gc, t_env_node *env)
{
	t_env_node *pwd_old;
	t_env_node *pwd_curr;
	
	pwd_curr = get_env_node(env, "PWD");
	pwd_old = get_env_node(env, "OLDPWD");
	pwd_old->variable = pwd_curr->variable; //OLDPWD takes the value of current pwd
	pwd_curr->variable = getcwd(NULL, 0);
    if (pwd_curr->variable == NULL)
        perror_exit(*gc, errno, "Failed to get current path");
    setter_gc(pwd_curr->variable, gc);
	return ;
}