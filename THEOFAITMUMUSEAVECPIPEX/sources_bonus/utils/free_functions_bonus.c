/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_functions_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 18:40:58 by tfreydie          #+#    #+#             */
/*   Updated: 2024/02/03 18:46:47 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

void	free_array_from_index(void **array, int j)
{
	while (j > 0)
	{
		j--;
		if (array[j])
			free(array[j]);
	}
	free(array);
	return ;
}

void	*free_all_cmds_n_args(char ***array, int j)
{
	int	i;

	while (j > 0)
	{
		i = 0;
		j--;
		if (array[j])
		{
			while (array[j][i])
			{
				free(array[j][i]);
				i++;
			}
		}
		free(array[j]);
	}
	free(array);
	return (NULL);
}

void	free_array(void **array)
{
	int	j;

	j = 0;
	while (array[j])
	{
		if (array[j])
			free(array[j]);
		j++;
	}
	free(array);
}

void	free_all_init_malloc(t_cmd *cmd_line)
{
	free_array_from_index((void **)cmd_line->fd, cmd_line->pipes);
	free_array((void **)cmd_line->possible_paths);
	free(cmd_line->child_ids);
	free_all_cmds_n_args(cmd_line->commands, cmd_line->command_number);
	safe_unlink(cmd_line);
	return ;
}

void	safe_unlink(t_cmd *cmd_line)
{
	if (access(".ft_heredoc", F_OK) == 0)
	{
		if (cmd_line->here_doc)
		{
			if (unlink(".ft_heredoc") == -1)
			{
				perror("failed to unlink heredoc");
				exit(errno);
			}
		}
	}
	return ;
}
