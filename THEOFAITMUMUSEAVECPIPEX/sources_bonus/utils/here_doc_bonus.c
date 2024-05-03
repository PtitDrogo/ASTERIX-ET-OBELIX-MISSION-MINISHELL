/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/02/03 18:46:49 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static int	ft_strncmp_pipex(char *s1, char *s2, size_t n);
static void	reopen_heredoc(t_cmd *cmd_line);
void		set_heredoc(char *arg, t_cmd *cmd_line);

void	here_doc(char *delimiter, t_cmd *cmd_line)
{
	char	*input;
	int		here_doc_fd;

	here_doc_fd = open(".ft_heredoc", O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (here_doc_fd < 0)
		perror_and_exit("failed to open heredoc", cmd_line);
	while (1)
	{
		write(1, "heredoc> ", 9);
		input = get_next_line(0);
		if (!input)
			perror_and_exit("failed to get line from heredoc", cmd_line);
		if (!ft_strncmp_pipex(input, delimiter, ft_strlen(delimiter)))
			break ;
		write(here_doc_fd, input, ft_strlen(input));
		free (input);
	}
	free(input);
	if (close(here_doc_fd) == -1)
		perror_and_exit("failed to close temp heredoc", cmd_line);
	reopen_heredoc(cmd_line);
	return ;
}

void	set_heredoc(char *arg, t_cmd *cmd_line)
{
	if (arg && !ft_strncmp_pipex("here_doc", arg, 9))
		cmd_line->here_doc = 1;
	else
		cmd_line->here_doc = 0;
}

static void	reopen_heredoc(t_cmd *cmd_line)
{
	cmd_line->infile = open(".ft_heredoc", O_RDONLY);
	if (cmd_line->infile < 0)
	{
		if (unlink(".ft_heredoc") == -1)
		{
			perror("failed to unlink heredoc after heredoc failed to open");
			exit(errno);
		}
		perror_and_exit("failed to open heredoc", cmd_line);
	}
}

static int	ft_strncmp_pipex(char *input, char *delimiter, size_t n)
{
	size_t	i;

	i = 0;
	while (input[i] == delimiter[i] && input[i] && i < n)
	{
		i++;
	}
	if (input[i] == '\n' && delimiter[i] == '\0')
		return (0);
	return ((unsigned char)input[i] - (unsigned char)delimiter[i]);
}
