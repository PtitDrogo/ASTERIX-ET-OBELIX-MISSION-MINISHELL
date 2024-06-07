/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 12:01:34 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/07 17:17:38 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strncmp_n(char *input, char *delim, size_t n)
{
	size_t	i;

	i = 0;
	while (input[i] == delim[i] && input[i] && i < n)
	{
		i++;
	}
	if (input[i] == '\n' && delim[i] == '\0')
		return (0);
	return ((unsigned char)input[i] - (unsigned char)delim[i]);
}

int	print_open_err_msg(int errnumber, char *file)
{
	if (errnumber == ENOENT)
		if (ft_printf2("bash: %s: No such file or directory\n", file) == -1)
			return (2);
	if (errnumber == EACCES)
		if (ft_printf2("bash: %s: Permission denied\n", file) == -1)
			return (2);
	if (errnumber == EISDIR)
		if (ft_printf2("bash: %s: Is a directory\n", file) == -1)
			return (2);
	if (errnumber == EMFILE)
		if (ft_printf2("bash: %s: Too many files opened", file) == -1)
			return (2);
	return (1);
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

void	handle_directory_case(char *valid_path, t_gc *gc)
{
	struct stat	filestat;

	if (valid_path == NULL)
		return ;
	stat(valid_path, &filestat);
	if (S_ISDIR(filestat.st_mode))
	{
		ft_printf2("bash: %s: Is a directory\n", valid_path);
		empty_trash_exit(gc, 126);
	}
	return ;
}
