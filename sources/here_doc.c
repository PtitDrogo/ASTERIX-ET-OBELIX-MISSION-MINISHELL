/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 17:01:16 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char		*readline_n_add_n(char *readline, t_gc **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);
static void		here_doc_process(t_data *data, char *delimiter, int fd, bool do_expand);

int parse_all_here_docs(t_data *data)
{
	t_token *current;
	t_cmd	*current_cmd;
	
	int		status;
	bool	do_expand;
	
	do_expand = true;
	status = EXIT_SUCCESS;
	global_gc(&data->gc);
	global_cmd(data->cmds);
	current_cmd = data->cmds;
	while (current_cmd)
	{
		current = current_cmd->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				int pipe_heredoc[2];
				pipe(pipe_heredoc);
				int before_expand_len = ft_len(current->next->str);
				current->next->str = expand_single_str(data, current->next->str, REMOVESQUOTES);
				if (before_expand_len != ft_len(current->next->str))
					do_expand = false;
				current->token_fd = pipe_heredoc[0];
				status = here_doc(data, current->next->str, pipe_heredoc[1], do_expand);
				close(pipe_heredoc[1]);
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

int	here_doc(t_data *data, char *delimiter, int fd, bool do_expand)
{
	int	status;
	int		pid;

	global_fd(fd);
	pid = fork();
	if (pid == -1)
		exit_heredoc(EXIT_FAILURE);
	else if (pid == 0)
	{
		signal(SIGINT, cancel_heredoc);
		here_doc_process(data, delimiter, fd, do_expand);
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

t_gc	**global_gc(t_gc **gc)
{
	static t_gc	**sgc;

	if (gc)
		sgc = gc;
	return (sgc);
}

t_cmd	*global_cmd(t_cmd *cmds)
{
	static t_cmd	*ccmds;

	if (cmds)
		ccmds = cmds;
	return (ccmds);
}

int	global_fd(int fd)
{
	static int	ffd;

	if (fd != -1)
		ffd = fd;
	return (ffd);
}

//here_doc that will write into the fd we give it, it doesnt update history because life is hard.
static void	here_doc_process(t_data *data, char *delimiter, int fd, bool do_expand)
{
	char	*input;

    while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), &data->gc);
		if (input == NULL)
		{	
			if (ft_printf2("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
			{
				free_heredoc();
				perror_exit(data->gc, errno, WRITE_ERR_MSG);
			}
			return ;
		}
		if (ft_strncmp_n(input, delimiter, ft_len(input)) == 0)
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

char	*readline_n_add_n(char *readline, t_gc **gc)
{
	if (readline == NULL)
		return (NULL);
	setter_gc(readline, gc);
	readline = ft_strjoin(readline, "\n");
	malloc_check(readline, *gc);
	setter_gc(readline, gc);
	return (readline);
}

static int    ft_strncmp_n(char *input, char *delimiter, size_t n)
{
    size_t    i;

    i = 0;
    while (input[i] == delimiter[i] && input[i] && i < n)
    {
        i++;
    }
    if (input[i] == '\n' && delimiter[i] == '\0')
        return (0);
	// printf("cmp is about to return %i\n",(unsigned char)input[i] - (unsigned char)delimiter[i] );
    return ((unsigned char)input[i] - (unsigned char)delimiter[i]);
}



