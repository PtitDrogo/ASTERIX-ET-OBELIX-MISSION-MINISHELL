/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_errors.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 16:15:19 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 16:25:49 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_errors_two(char *dir_path, t_gc *gc);
static void	handle_errors_one(char *dir_path, t_gc *gc);

void	handle_error(char *dir_path, t_gc *gc)
{
	handle_errors_one(dir_path, gc);
	handle_errors_two(dir_path, gc);
	return ;
}

static void	handle_errors_one(char *dir_path, t_gc *gc)
{
	if (errno == EACCES)
	{
		if (ft_printf2("bash: cd: %s: \
			Permission denied\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else if (errno == ENOENT)
	{
		if (ft_printf2("bash: cd: %s: \
			No such file or directory\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
}

static void	handle_errors_two(char *dir_path, t_gc *gc)
{
	if (errno == ENOTDIR)
	{
		if (ft_printf2("bash: cd: %s: \
			Not a directory\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else if (errno == EFAULT)
	{
		if (ft_printf2("bash: cd: %s: \
			Path outside of range of process\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
	else
	{
		if (ft_printf2("bash: cd: %s: \
			Couldn't change directory\n", dir_path) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
	}
}
