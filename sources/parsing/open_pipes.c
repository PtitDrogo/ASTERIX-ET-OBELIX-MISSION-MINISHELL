/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_pipes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 13:59:06 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/07 04:10:25 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_pipes(t_token *token_list);
static void	init_pipes(int **pipes,int pipe_number, t_garbage_collect *gc);
static int	**malloc_pipes_fds(int pipe_number, t_garbage_collect **gc);
static void	fill_pipes_in_token(t_cmd *cmds, int **pipes_fds);

//open all pipes, fill cmd with proper pipes and return the **ptr to close them easily later;
int **open_pipes(t_cmd *cmds, t_garbage_collect **gc, int number_of_pipes)
{
	int 	**pipes_fds;

	pipes_fds = malloc_pipes_fds(number_of_pipes, gc);
	init_pipes(pipes_fds, number_of_pipes, *gc);
	fill_pipes_in_token(cmds, pipes_fds);

	return(pipes_fds);
}

static void fill_pipes_in_token(t_cmd *cmds, int **pipes_fds)
{
	//ASSUMING A PERFECT TOKEN FUNCTION (SPOILER, ITS NOT)
	int i;
	
	i = -1; //je suis remplis de sournoiseries;
	while (cmds)
	{
		printf("hihi current cmds is %s and cmd in is %p\n", cmds->str[0], cmds->redirection_in);
		if ( cmds->redirection_in)
			printf("cnd redir in type is %u\n", cmds->redirection_in->type);
		if (cmds->redirection_in && cmds->redirection_in->type == PIPE)
		{	
			cmds->redirection_in->pipe_fd = pipes_fds[i][0];
			printf("cmd %s has redirection fd of type %u and of fd %i\n", cmds->str[0], cmds->redirection_in->type, cmds->redirection_in->pipe_fd);
		}
		i++; // C'est de la triche mais en theorie ca passe;
		if (cmds->redirection_out && cmds->redirection_out->type == PIPE)
		{	
			printf("current cmd is %s\n", cmds->str[0]);
			cmds->redirection_out->pipe_fd = pipes_fds[i][1];
		}
		cmds = cmds->next;
	}  
}

int count_pipes(t_token *token_list)
{
	int pipe_count;
	
	pipe_count = 0;
	while (token_list)
	{
		if (token_list->type == PIPE)
			pipe_count++;
		token_list = token_list->next;
	}
	return (pipe_count);
}

//call the pipes function to fill the **int full of fds;
static void	init_pipes(int **pipes,int pipe_number, t_garbage_collect *gc)
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
//malloc enough spaces for pipes fds;
static int	**malloc_pipes_fds(int pipe_number, t_garbage_collect **gc)
{
	int	i;
	int **pipe_fds;

	i = 0;
	pipe_fds = (int **)malloc_trash(sizeof(int *) * pipe_number, gc);
	while (i < pipe_number)
	{
		pipe_fds[i] = (int *)malloc_trash(sizeof(int) * 2, gc);
		i++;
	}
	return (pipe_fds);
}