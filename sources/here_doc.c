/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/23 19:26:21 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, somehow get the initial call to heredoc to the history
static char		*readline_n_add_n(char *readline, t_garbage_collect **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);
static void		child_here_doc(char *delimiter, t_garbage_collect **gc, int fd);
// int				new_here_doc_check(t_token *here_doc_token, int **pipe, t_garbage_collect **gc);
int				count_here_docs(t_cmd *cmds);


// void parse_all_here_docs(t_cmd *cmds, t_garbage_collect **gc)
// {
// 	int number_of_pipes;
// 	int **pipe_array;
// 	int	i;
	
// 	i = 0;
// 	number_of_pipes = count_here_docs(cmds);
// 	if (number_of_pipes == 0)
// 		return ;
// 	pipe_array = malloc_trash(sizeof(int *) * (number_of_pipes + 1), gc);
// 	pipe_array[number_of_pipes] = NULL;
// 	while (cmds)
// 	{
// 		if (cmds->type == D_LESS)
// 		{
// 			pipe_array[i] = malloc_trash(sizeof(int) * 2, gc);
// 			pipe(pipe_array[i]);
// 			here_doc(cmds->next->str, gc, pipe_array[i][1]);
// 			cmds->here_doc_pipe = pipe_array[i][0];
// 			close(pipe_array[i][1]);
// 			printf("In parse all here doc\n");
// 			printf("the value of token heredoc pipe is %i\n", cmds->here_doc_pipe);
// 			check_fd(cmds->here_doc_pipe);
// 			i++;
// 		}
// 		cmds = cmds->next;
// 	}
// 	return ;
// }

// int	count_here_docs(t_cmd *cmds)
// {

// 	int i;

// 	i = 0;
// 	while (cmds)
// 	{
// 		if (cmds->type == D_LESS)
// 			i++;
// 		cmds = cmds->next;
// 	}
// 	return (i);
	
// }

int	here_doc(char *delimiter, t_garbage_collect **gc, int fd)
{
	int	fork_id;
	int	status;

	global_gc(gc);
	global_fd(fd);
	signal(SIGINT, cancel_heredoc);
	here_doc_process(delimiter, gc, fd);
	/*fork_id = fork();
	if (fork_id == -1)
		empty_trash_exit(*gc, 0);
	if (fork_id == 0)
		child_here_doc(delimiter, gc, fd);
	else
	{
		waitpid(fork_id, &status, 0);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		else
			status = 0;
	}
	ft_printf("status returned : %i ", status);
	return (status);*/
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
static void	here_doc_process(char *delimiter, t_garbage_collect **gc, int fd)
{
	char	*input;

    while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), gc);
		if (input == NULL)
			if (ft_printf_err("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
				exit(EXIT_SUCCESS);//return (0);//TODO MAKE UP MY MIND //CTRL D doesnt exit shell just the current heredoc; (if its the last here_doc it exits the shell tho);
        if (ft_strncmp_n(input, delimiter, ft_strlen(input)) == 0)
		{	
			// printf("I am breaking out of heredoc loop'n");
			break ;
		}
		if (write(fd, input, ft_strlen(input)) == -1)
            perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	printf("Returning 1 in heredoc\n");
	exit(EXIT_SUCCESS);
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

