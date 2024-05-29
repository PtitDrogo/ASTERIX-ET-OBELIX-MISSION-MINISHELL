/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/29 18:18:28 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, somehow get the initial call to heredoc to the history
static char		*readline_n_add_n(char *readline, t_garbage_collect **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);
static void		here_doc_process(char *delimiter, t_garbage_collect **gc, int fd, bool do_expand, t_env_node *env, char *error_value);
char 			*expand_here_doc_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value);
char 			*expand_here_doc_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value);

//I want this function to exit with only one open pipe end per USED heredoc;
void parse_all_here_docs(t_cmd *cmds, t_garbage_collect **gc, t_env_node *env, char *error_value)
{
	t_token *current;
	bool do_expand;
	
	do_expand = true;
	while (cmds)
	{
		current = cmds->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				int pipe_heredoc[2];
				pipe(pipe_heredoc);
				//need to do a remove quote on current_next_str
				int before_expand_len = ft_strlen(current->next->str);
				current->next->str = remove_quotes(gc, current->next->str);
				if (before_expand_len != ft_strlen(current->next->str))
					do_expand = false; 
				here_doc(current->next->str, gc, pipe_heredoc[1], do_expand, env, error_value);
				current->here_doc_pipe = pipe_heredoc[0];
				close(pipe_heredoc[1]);
				check_fd(current->here_doc_pipe);
				current = current->next;
			}
			current = current->next;
		}
		cmds = cmds->next;
	}
}

int	here_doc(char *delimiter, t_garbage_collect **gc, int fd, bool do_expand, t_env_node *env, char *error_value)
{
	int	fork_id;
	int	status;

	global_gc(gc);
	global_fd(fd);
	signal(SIGINT, cancel_heredoc);
	here_doc_process(delimiter, gc, fd, do_expand, env, error_value);
	return (0);
}

t_garbage_collect	**global_gc(t_garbage_collect **gc)
{
	static t_garbage_collect	**sgc;

	if (gc)
		sgc = gc;
	return (sgc);
}

int	global_fd(int fd)
{
	static int	ffd;

	if (fd)
		ffd = fd;
	return (ffd);
}

//here_doc that will write into the fd we give it, it doesnt update history because life is hard.
static void	here_doc_process(char *delimiter, t_garbage_collect **gc, int fd, bool do_expand, t_env_node *env, char *error_value)
{
	char	*input;

    while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), gc);
		if (input == NULL)
		{	
			if (ft_printf_err("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
			{	
				close(fd); //we should close all fds we managed to make, wheres is my fd linked list
				perror_exit(*gc, errno, WRITE_ERR_MSG);
			}
			return ;
		}
		if (ft_strncmp_n(input, delimiter, ft_strlen(input)) == 0)
			break ;
		//IF delimiter isnt in quote we expand EVERYTHING
		// input = expand_here_doc_str(input); //WILL NEED TO FEED A STRUCT WITH ALL the Variables needed;
		if (do_expand == true)
			input = expand_here_doc_str(env, gc, input, error_value);
		
		if (write(fd, input, ft_strlen(input)) == -1)
        {    
			close(fd); ///we should close all fds we managed to make, wheres is my fd linked list
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		}
	}
	printf("Returning 1 in heredoc\n");
	// exit(EXIT_SUCCESS);
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



