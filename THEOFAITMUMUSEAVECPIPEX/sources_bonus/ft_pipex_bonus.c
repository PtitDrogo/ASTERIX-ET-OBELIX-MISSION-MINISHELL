/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pipex_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 18:40:58 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/30 16:12:25 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static void	child_process(int argc, char *argv[], char **envp, t_cmd *cmd_line);
static void	process_behavior(int argc, char *argv[], t_cmd *cmd_line, int pos);

int	main(int argc, char *argv[], char **envp)
{
	t_cmd		cmd_line;
	int			i;

	if (argc == 1 || argc < 5 + (ft_strncmp(argv[1], "here_doc", 8) == 0))
		return (perror("invalid number of arguments"), 1);
	init_all(argc, argv, &cmd_line, envp);
	i = -1;
	//En soit je peux savoir le nombre de commande si j'ai la string entiere (nombre de Pipe + 1)
	while (++i < cmd_line.command_number)
	{
		cmd_line.current_process = i;
		child_process(argc, argv, envp, &cmd_line);
	}
	i = -1;
	close_all_pipes(&cmd_line);
	while (++i < cmd_line.command_number)
	{
		if (waitpid(cmd_line.child_ids[i], &cmd_line.status, 0) == -1)
			free_all_and_exit(&cmd_line, "error waiting for children");
	}

	free_all_init_malloc(&cmd_line);
	return (WEXITSTATUS(cmd_line.status));
}

static void	child_process(int argc, char *argv[], char **envp, t_cmd *cmd)
{
	//we fork and check if it fails
	cmd->child_ids[cmd->current_process] = fork();
	if (cmd->child_ids[cmd->current_process] == -1)
		free_all_and_exit(cmd, "Error forking");
	if (cmd->child_ids[cmd->current_process] == 0)
	{
		process_behavior(argc, argv, cmd, cmd->current_process);
		close_all_pipes(cmd);
		// NEW BELOW// I should put this in its own function but its pushed and im lazy
		if (cmd->current_process == 0)
		{
			if (close(cmd->infile) == -1)
				free_all_and_exit(cmd, "Error closing fd in child");
		}
		if (cmd->current_process == cmd->command_number -1)
		{
			if (close(cmd->outfile) == -1)
				free_all_and_exit(cmd, "Error closing fd in child");
		}
		////NEW ABOVE^^^^
		cmd->valid_path = find_valid_path(cmd);
		if (cmd->valid_path)
			execve(cmd->valid_path, cmd->commands[cmd->current_process], envp);
		free(cmd->valid_path);
		free_all_and_exit(cmd, "Error executing child/command not found");
	}
	if (cmd->child_ids[cmd->current_process] > 0 && cmd->current_process != 0)
		cmd->current_pipe++;
	return ;
}

//this entire part is probably whats gonna be rewritten the most if i had to imagine
// by default cmds will read and write to std unless told otherwise.
//so this should be the behavior of my commands unless I redirect them with Pipes or redirections
static void	process_behavior(int argc, char *argv[], t_cmd *cmd, int pos)
{
	int	out;

	if (pos == 0)
	{
		if (!cmd->infile)
			cmd->infile = open(argv[1], O_RDONLY);
		secure_dup2(cmd->infile, STDIN_FILENO, cmd);
		secure_dup2(cmd->fd[cmd->current_pipe][1], STDOUT_FILENO, cmd);
	}
	else if ((pos + 1) == cmd->command_number)
	{
		out = argc - 1;
		if (cmd->here_doc)
			cmd->outfile = open(argv[out], O_WRONLY | O_APPEND | O_CREAT, 0644);
		else
			cmd->outfile = open(argv[out], O_WRONLY | O_TRUNC | O_CREAT, 0644);
		secure_dup2(cmd->outfile, STDOUT_FILENO, cmd);
		secure_dup2(cmd->fd[cmd->current_pipe][0], STDIN_FILENO, cmd);
	}
	else
	{
		secure_dup2(cmd->fd[cmd->current_pipe + 1][1], STDOUT_FILENO, cmd);
		secure_dup2(cmd->fd[cmd->current_pipe][0], STDIN_FILENO, cmd);
	}
	return ;
}

void	close_all_pipes(t_cmd *cmd_line)
{
	int	i;

	i = 0;
	while (i < cmd_line->pipes)
	{
		if (close(cmd_line->fd[i][0]) == -1)
			free_all_and_exit(cmd_line, "Error closing fd in child");
		if (close(cmd_line->fd[i][1]) == -1)
			free_all_and_exit(cmd_line, "Error closing fd in child");
		i++;
	}
	return ;
}
