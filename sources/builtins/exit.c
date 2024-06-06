/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:57:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 16:34:42 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	too_many_arguments(char **str);
static bool	char_check(char *str);
static int	get_exit_return_value(char *arg);
static int	handle_too_many_args(t_gc **gc, int backup_fds[2]);

//Make up your mind with error messages;
int	ft_exit(char **args, t_gc *gc, int backup_fds[2])
{
	if (args == NULL || *args == NULL || *args[0] == '\0')
	{
		close_backup_fds(backup_fds);
		if (ft_printf("exit\n") == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(gc, 0);
	}
	if (char_check(*args) == true || ft_safe_atoi(*args) == ATOI_ERROR)
	{
		close_backup_fds(backup_fds);
		if (ft_printf("exit\n") == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		if (ft_printf2("bash: exit: %s: numeric \
				argument required\n", args[0]) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(gc, 2);
	}
	if (too_many_arguments(args) == true)
		return (handle_too_many_args(&gc, backup_fds));
	close_backup_fds(backup_fds);
	empty_trash_exit(gc, get_exit_return_value(*args));
	return (0);
}

static int	get_exit_return_value(char *arg)
{
	int	result;

	result = ft_atoi(arg);
	if (result < 0)
	{
		while (result < 0)
			result += 256;
	}
	else
		result = result % 256;
	return (result);
}

static bool	char_check(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i] && str[i] != ' ')
	{
		if (ft_isdigit(str[i]) == false)
			return (true);
		i++;
	}
	return (false);
}

static bool	too_many_arguments(char **str)
{
	if (str[1] == NULL)
		return (false);
	return (true);
}

static int	handle_too_many_args(t_gc **gc, int backup_fds[2])
{
	if (ft_printf("exit\n") == -1)
	{
		close_backup_fds(backup_fds);
		perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	if (ft_printf2("bash: exit: too many arguments\n") == -1)
	{
		close_backup_fds(backup_fds);
		perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	return (1);
}
