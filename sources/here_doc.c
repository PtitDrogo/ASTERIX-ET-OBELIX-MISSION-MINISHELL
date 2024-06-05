/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 16:17:31 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char		*readline_n_add_n(char *readline, t_gc **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);
static void		here_doc_process(char *delimiter, t_gc **gc, int fd, bool do_expand, t_env *env, char *error_value);
char 			*expand_here_doc_str(t_env *env, t_gc **gc, char *array, char *error_value);
char 			*expand_here_doc_str(t_env *env, t_gc **gc, char *array, char *error_value);

int parse_all_here_docs(t_cmd *cmds, t_gc **gc, t_env *env, char *error_value)
{
	t_token *current;
	int		status;
	bool do_expand;
	
	do_expand = true;
	status = EXIT_SUCCESS;
	global_gc(gc);
	global_cmd(cmds);
	while (cmds)
	{
		current = cmds->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				int pipe_heredoc[2];
				pipe(pipe_heredoc);
				int before_expand_len = ft_len(current->next->str);
				current->next->str = expand_single_str(env ,gc, current->next->str, error_value, REMOVESQUOTES);
				if (before_expand_len != ft_len(current->next->str))
					do_expand = false;
				current->token_fd = pipe_heredoc[0];
				status = here_doc(current->next->str, gc, pipe_heredoc[1], do_expand, env, error_value);
				close(pipe_heredoc[1]);
				if (status != EXIT_SUCCESS)
					return (status);
				current = current->next;
			}
			current = current->next;
		}
		cmds = cmds->next;
	}
	return (status);
}

int	here_doc(char *delimiter, t_gc **gc, int fd, bool do_expand, t_env *env, char *error_value)
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
		here_doc_process(delimiter, gc, fd, do_expand, env, error_value);
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
static void	here_doc_process(char *delimiter, t_gc **gc, int fd, bool do_expand, t_env *env, char *error_value)
{
	char	*input;

    while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), gc);
		if (input == NULL)
		{	
			if (ft_printf2("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
			{
				free_heredoc();
				perror_exit(*gc, errno, WRITE_ERR_MSG);
			}
			return ;
		}
		if (ft_strncmp_n(input, delimiter, ft_len(input)) == 0)
			break ;
		if (do_expand == true)
			input = expand_single_str(env, gc, input, error_value, EXPAND);	
		if (write(fd, input, ft_len(input)) == -1)
		{
			free_heredoc();
            perror_exit(*gc, errno, WRITE_ERR_MSG);
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



