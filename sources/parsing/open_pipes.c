/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_pipes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 13:59:06 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/03 06:28:33 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			count_pipes(t_token *token_list);
static void	init_pipes(int **pipes, int pipe_number, t_gc *gc);
static int	**malloc_pipes_fds(int pipe_number, t_gc **gc);
static void	fill_pipes_in_token(t_cmd *cmds, int **pipes_fds);

int	**open_pipes(t_cmd *cmds, t_gc **gc, int number_of_pipes)
{
	int	**pipes_fds;

	//Technically if the malloc fails we need to close the pipes fds;
	pipes_fds = malloc_pipes_fds(number_of_pipes, gc);
	init_pipes(pipes_fds, number_of_pipes, *gc);
	fill_pipes_in_token(cmds, pipes_fds);
	return (pipes_fds);
}

static void	fill_pipes_in_token(t_cmd *cmds, int **pipes_fds)
{
	int	i;

	i = -1;
	while (cmds)
	{
		if (cmds->redirection_in && cmds->redirection_in->type == PIPE)
			cmds->redirection_in->token_fd = pipes_fds[i][0];
		i++;
		if (cmds->redirection_out && cmds->redirection_out->type == PIPE)
			cmds->redirection_out->token_fd = pipes_fds[i][1];
		cmds = cmds->next;
	}
}

int	count_pipes(t_token *token_list)
{
	int	pipe_count;

	pipe_count = 0;
	while (token_list)
	{
		if (token_list->type == PIPE)
			pipe_count++;
		token_list = token_list->next;
	}
	return (pipe_count);
}

static void	init_pipes(int **pipes,int pipe_number, t_gc *gc)
{
	int	i;

	i = 0;
	while (i < pipe_number)
	{
		if (pipe(pipes[i]) == -1)
			perror_exit(gc, errno, "Failed to open pipe");
		i++;
	}
	return ;
}

static int	**malloc_pipes_fds(int pipe_number, t_gc **gc)
{
	int	i;
	int	**pipe_fds;

	i = 0;
	pipe_fds = (int **)malloc_trash(sizeof(int *) * pipe_number, gc);
	while (i < pipe_number)
	{
		pipe_fds[i] = (int *)malloc_trash(sizeof(int) * 2, gc);
		i++;
	}
	return (pipe_fds);
}