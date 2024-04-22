/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:57:56 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/22 18:56:35 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, replace errors with write to stderr;

static bool	too_many_arguments(char **str);
static bool	is_letter_in_first_arg(char *str);
static int	get_exit_return_value(char *arg);
static void	empty_trash_exit(t_garbage_collect *gc, int exit_code);

int ft_exit(char **args, t_garbage_collect *gc)
{
    int exit_value;
    int i;

    if (args == NULL || *args == NULL || *args[0] == '\0')
    {
		if (printf("exit\n") == -1)
			empty_trash_exit(gc, EXIT_FAILURE);
		// printf("exit with code 0\n");
		empty_trash_exit(gc, 0); // normal execution
	}
	if (is_letter_in_first_arg(*args) == true || ft_safe_atoi(*args) == ATOI_ERROR) //hilarious use of my safe atoi can perfectly replicate bash
	{
		if (printf("exit\nbash: exit: %s: numeric argument required\n", args[0]) == -1)
			empty_trash_exit(gc, EXIT_FAILURE);
		// printf("exit with code 2\n");
		empty_trash_exit(gc, 2);
	}
    if (too_many_arguments(args) == true)
	{
		if (printf("exit\nbash: exit: too many arguments\n") == -1)
			empty_trash_exit(gc, EXIT_FAILURE);
		// printf("does not exit with code 1\n");
		return (1); // this does NOT exit
	}
	empty_trash_exit(gc, get_exit_return_value(*args));
	return(0); // we should never get there
}
//error message should be written in 2 so the printf will have to go;

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
	// printf("exit with code %i\n", result);
	return (result);
	
}
static bool	is_letter_in_first_arg(char *str)
{
	int i;

	i = 0;
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

static void	empty_trash_exit(t_garbage_collect *gc, int exit_code)
{
	empty_trash(gc);
	rl_clear_history();
	exit(exit_code);
}
