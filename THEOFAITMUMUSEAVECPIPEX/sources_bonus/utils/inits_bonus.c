/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inits_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 05:48:38 by tfreydie          #+#    #+#             */
/*   Updated: 2024/02/06 20:13:08 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static void		init_mallocs(t_cmd *cmd_line, char *argv[]);
static pid_t	*init_child_ids(t_cmd *cmd_line);
static int		**init_fds(int **fds, t_cmd *cmd_line);
static void		init_pipes(t_cmd *cmd_line);

t_cmd	*init_all(int argc, char *argv[], t_cmd *cmd_line, char **envp)
{
	cmd_line->infile = 0;
	cmd_line->outfile = 0;
	set_heredoc(argv[1], cmd_line);
	if (cmd_line->here_doc)
		here_doc(argv[2], cmd_line);
	cmd_line->command_number = argc - 3 - cmd_line->here_doc;
	cmd_line->pipes = argc - 4 - cmd_line->here_doc;
	cmd_line->current_pipe = 0;
	cmd_line->possible_paths = ft_split(find_env_variable(envp, "PATH="), ':');
	init_mallocs(cmd_line, argv);
	init_pipes(cmd_line);
	return (cmd_line);
}

static pid_t	*init_child_ids(t_cmd *cmd_line)
{
	pid_t	*result;

	result = malloc(sizeof(pid_t) * cmd_line->command_number);
	if (!result)
		return (NULL);
	return (result);
}

static int	**init_fds(int **fds, t_cmd *cmd_line)
{
	int	i;

	i = 0;
	fds = (int **)malloc(sizeof(int *) * cmd_line->pipes);
	if (!fds)
		return (NULL);
	while (i < cmd_line->pipes)
	{
		fds[i] = (int *)malloc(sizeof(int) * 2);
		if (!fds[i])
		{
			free_array_from_index((void **)fds, i);
			return (NULL);
		}
		i++;
	}
	return (fds);
}

static void	init_pipes(t_cmd *cmd_line)
{
	int	i;

	i = 0;
	while (i < cmd_line->pipes)
	{
		if (pipe(cmd_line->fd[i]) == -1)
			free_all_and_exit(cmd_line, "Error during init of pipes fds array");
		i++;
	}
	return ;
}

static void	init_mallocs(t_cmd *cmd_line, char *argv[])
{
	if (!cmd_line->possible_paths)
		perror_and_exit("error splitting PATH", cmd_line);
	cmd_line->child_ids = init_child_ids(cmd_line);
	if (!cmd_line->child_ids)
	{
		free_array((void **)cmd_line->possible_paths);
		perror_and_exit("error during child ids malloc", cmd_line);
	}
	cmd_line->commands = ft_arg_parsing(argv, cmd_line);
	if (!cmd_line->commands)
	{
		free_array((void **)cmd_line->possible_paths);
		free(cmd_line->child_ids);
		perror_and_exit("error during parsing of arguments", cmd_line);
	}
	cmd_line->fd = init_fds(cmd_line->fd, cmd_line);
	if (!cmd_line->fd)
	{
		free_all_cmds_n_args(cmd_line->commands, cmd_line->command_number);
		free_array((void **)cmd_line->possible_paths);
		free(cmd_line->child_ids);
		perror_and_exit("error during init of fd", cmd_line);
	}
}
