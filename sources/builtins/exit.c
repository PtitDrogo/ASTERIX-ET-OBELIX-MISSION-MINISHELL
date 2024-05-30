/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:57:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 05:29:02 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, replace errors with write to stderr;

static bool	too_many_arguments(char **str);
static bool	is_letter_in_first_arg(char *str);
static int	get_exit_return_value(char *arg);
void	close_backup_fds(int backup_fds[2]);

int ft_exit(char **args, t_garbage_collect *gc, int backup_fds[2])
{
    int exit_value;
    int i;

	
    if (args == NULL || *args == NULL || *args[0] == '\0')
    {
		close_backup_fds(backup_fds);
		if (printf("exit\n") == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(gc, 0); // normal execution
	}
	if (is_letter_in_first_arg(*args) == true || ft_safe_atoi(*args) == ATOI_ERROR) //hilarious use of my safe atoi can perfectly replicate bash
	{
		close_backup_fds(backup_fds);
		if (printf("exit\n") == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		if (ft_printf2("bash: exit: %s: numeric argument required\n", args[0]) == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		empty_trash_exit(gc, 2);
	}
    if (too_many_arguments(args) == true)
	{
		if (printf("exit\n") == -1)
			perror_exit(gc, errno, WRITE_ERR_MSG);
		if (ft_printf2("bash: exit: too many arguments\n") == -1)
		{	
			close_backup_fds(backup_fds);
			perror_exit(gc, errno, WRITE_ERR_MSG);
		}
		return (1); // this does NOT exit
	}
	close_backup_fds(backup_fds);
	empty_trash_exit(gc, get_exit_return_value(*args));
	return(0); // we should never get there
}
//error message should be written in 2 so the ft_printf2 will have to go;

static int		get_exit_return_value(char *arg)
{
	int result;
	result = ft_atoi(arg);
	if (result < 0)
	{
		while (result < 0)
			result += 256;
	}
	else
		result = result % 256;
	// ft_printf2("exit with code %i\n", result);
	return (result);
	
}
static bool	is_letter_in_first_arg(char *str)
{
	int i;

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

void	close_backup_fds(int backup_fds[2])
{
	if (backup_fds == NULL)
		return ;
	else
	{
		close (backup_fds[0]);
		close (backup_fds[1]);
	}
	return ;
}
