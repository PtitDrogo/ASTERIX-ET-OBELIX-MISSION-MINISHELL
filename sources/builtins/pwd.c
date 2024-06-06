/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 19:05:05 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 15:32:14 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	pwd(t_gc **gc)
{
	char	*pwd;
	char	*backup_pwd;

	backup_pwd = "hello I am backup pwd";
	pwd = getcwd(NULL, 0);
	setter_gc(pwd, gc);
	if (pwd)
	{
		if (printf("%s\n", pwd) == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	else
	{
		if (printf("%s\n", backup_pwd) == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	return (1);
}
