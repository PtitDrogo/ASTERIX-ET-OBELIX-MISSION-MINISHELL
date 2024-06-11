/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 19:05:05 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/11 15:34:09 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_extra_arg(t_gc **gc, char const *argv[]);

int	pwd(t_gc **gc)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	setter_gc(pwd, gc);
	if (pwd)
	{
		if (printf("%s\n", pwd) == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	else
	{
		if (ft_printf2("pwd: error retrieving current directory: getcwd: cannot \
				access parent directories: No such file or directory\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	return (1);
}

void	get_init_pwd(t_gc **gc, t_env **env, int argc, char const *argv[])
{
	t_env	*pwd_node;
	char	*pwd_str;

	if (argc > 1)
		handle_extra_arg(gc, argv);
	(void)argv;
	pwd_node = get_env_node(*env, "PWD");
	if (pwd_node != NULL)
		return ;
	pwd_str = getcwd(NULL, 0);
	setter_gc(pwd_str, gc);
	if (pwd_str == NULL)
	{
		if (ft_printf2("pwd: error retrieving current directory: getcwd: cannot \
				access parent directories: No such file or directory\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	else
	{
		pwd_str = setter_gc(ft_strjoin("PWD=", pwd_str), gc);
		malloc_check(pwd_str, *gc);
		export(env, pwd_str, gc);
	}
}

static void	handle_extra_arg(t_gc **gc, char const *argv[])
{
	if (ft_printf2("bash: %s: No such file or directory\n", argv[1]) == -1)
		perror_exit(*gc, errno, WRITE_ERR_MSG);
	empty_trash_exit(*gc, 127);
	return ;
}
