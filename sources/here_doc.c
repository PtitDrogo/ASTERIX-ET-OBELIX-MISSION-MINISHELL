/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 02:25:30 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, somehow get the initial call to heredoc to the history
static char		*readline_n_add_n(char *readline, t_garbage_collect **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);
static void		child_here_doc(char *delimiter, t_garbage_collect **gc, int fd);
// int				new_here_doc_check(t_token *here_doc_token, int **pipe, t_garbage_collect **gc);
static void	here_doc_process(char *delimiter, t_garbage_collect **gc, int fd);

//I want this function to exit with only one open pipe end per USED heredoc;
int parse_all_here_docs(t_cmd *cmds, t_garbage_collect **gc)
{
	t_token *current;
	int		status;
	// int	last_here_doc;

	status = EXIT_SUCCESS;
	global_gc(gc);
	global_cmd(cmds);
	while (cmds)
	{
		// last_here_doc = -1;
		current = cmds->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				int pipe_heredoc[2];
				// if (last_here_doc != -1)
				// 	close(last_here_doc);
				pipe(pipe_heredoc);
				status = here_doc(current->next->str, gc, pipe_heredoc[1]);//wallah
				current->here_doc_pipe = pipe_heredoc[0];
				close(pipe_heredoc[1]);
				check_fd(current->here_doc_pipe);
				if (status != EXIT_SUCCESS)
					return (status);
				// last_here_doc = pipe_heredoc[0];
				current = current->next;
			}
			current = current->next;
		}
		cmds = cmds->next;
	}
	return (status);
}

int	here_doc(char *delimiter, t_garbage_collect **gc, int fd)
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
		here_doc_process(delimiter, gc, fd);
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

t_garbage_collect	**global_gc(t_garbage_collect **gc)
{
	static t_garbage_collect	**sgc;

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
static void	here_doc_process(char *delimiter, t_garbage_collect **gc, int fd)
{
	char	*input;

    while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), gc);
		if (input == NULL)
		{	
			if (ft_printf_err("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
			{
				free_heredoc();
				perror_exit(*gc, errno, WRITE_ERR_MSG);
			}
			return ;
		}
		if (ft_strncmp_n(input, delimiter, ft_strlen(input)) == 0)
			break ;
		//if we want to expand before writing in here doc it would be here;
		if (write(fd, input, ft_strlen(input)) == -1)
		{
			free_heredoc();
            perror_exit(*gc, errno, WRITE_ERR_MSG);
		}
	}
	printf("Returning 1 in heredoc\n");
}

char	*readline_n_add_n(char *readline, t_garbage_collect **gc)
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

