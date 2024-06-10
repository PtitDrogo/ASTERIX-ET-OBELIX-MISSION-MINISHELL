/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/10 14:06:16 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int				ft_strncmp_n(char *input, char *delim, size_t n);
static void		hd_process(t_data *data, char *delim, int fd, bool do_expand);
bool			update_expand_bool(t_data *data, t_token *current);
int				execute_heredoc(t_data *data, t_token *current, bool do_expand);

int	parse_all_here_docs(t_data *data)
{
	t_token	*current;
	t_cmd	*current_cmd;
	int		status;
	bool	do_expand;

	innit_here_doc(&data->gc, data->cmds, &status);
	current_cmd = data->cmds;
	while (current_cmd)
	{
		current = current_cmd->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				do_expand = update_expand_bool(data, current);
				status = execute_heredoc(data, current, do_expand);
				if (status != EXIT_SUCCESS)
					return (status);
				current = current->next;
			}
			current = current->next;
		}
		current_cmd = current_cmd->next;
	}
	return (status);
}

int	execute_heredoc(t_data *data, t_token *current, bool do_expand)
{
	int	pipe_heredoc[2];
	int	status;

	pipe(pipe_heredoc);
	current->token_fd = pipe_heredoc[0];
	status = here_doc(data, current->next->str, pipe_heredoc[1], do_expand);
	close(pipe_heredoc[1]);
	return (status);
}

bool	update_expand_bool(t_data *data, t_token *current)
{
	int	before_expand_len;

	before_expand_len = ft_len(current->next->str);
	current->next->str = expand_single_str(data,
			current->next->str, REMOVESQUOTES);
	if (before_expand_len != ft_len(current->next->str))
		return (false);
	return (true);
}

int	here_doc(t_data *data, char *delim, int fd, bool do_expand)
{
	int		status;
	int		pid;

	set_n_get_fd(fd);
	pid = fork();
	if (pid == -1)
		exit_heredoc(EXIT_FAILURE);
	else if (pid == 0)
	{
		signal(SIGINT, cancel_heredoc);
		signal(SIGQUIT, SIG_IGN);
		hd_process(data, delim, fd, do_expand);
		exit_heredoc(EXIT_SUCCESS);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		status = 128 + WTERMSIG(status);
	if (status == 130)
		ft_printf("\n");
	return (status);
}

static void	hd_process(t_data *data, char *delim, int fd, bool do_expand)
{
	char	*input;

	while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), &data->gc);
		if (input == NULL)
		{
			if (ft_printf2("bash: warning: here-document delimited \
				by end-of-file (wanted `%s')\n", delim) == -1)
			{
				free_heredoc();
				perror_exit(data->gc, errno, WRITE_ERR_MSG);
			}
			return ;
		}
		if (ft_strncmp_n(input, delim, ft_len(input)) == 0)
			break ;
		if (do_expand == true)
			input = expand_single_str(data, input, EXPAND);
		if (write(fd, input, ft_len(input)) == -1)
		{
			free_heredoc();
			perror_exit(data->gc, errno, WRITE_ERR_MSG);
		}
	}
}
