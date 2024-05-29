/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/29 12:49:47 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, somehow get the initial call to heredoc to the history
static char		*readline_n_add_n(char *readline, t_garbage_collect **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);
static void		child_here_doc(char *delimiter, t_garbage_collect **gc, int fd);
// int				new_here_doc_check(t_token *here_doc_token, int **pipe, t_garbage_collect **gc);
static void	here_doc_process(char *delimiter, t_garbage_collect **gc, int fd);
char *expand_here_doc_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value);

//I want this function to exit with only one open pipe end per USED heredoc;
void parse_all_here_docs(t_cmd *cmds, t_garbage_collect **gc)
{
	t_token *current;
	
	while (cmds)
	{
		current = cmds->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				int pipe_heredoc[2];
				pipe(pipe_heredoc);
				here_doc(current->next->str, gc, pipe_heredoc[1]);
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

int	here_doc(char *delimiter, t_garbage_collect **gc, int fd)
{
	int	fork_id;
	int	status;

	global_gc(gc);
	global_fd(fd);
	signal(SIGINT, cancel_heredoc);
	here_doc_process(delimiter, gc, fd);
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
		{	
			if (ft_printf_err("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
				perror_exit(*gc, errno, WRITE_ERR_MSG);
			return ;
		}
		if (ft_strncmp_n(input, delimiter, ft_strlen(input)) == 0)
			break ;
		//if we want to expand before writing in here doc it would be here;
		// input = expand_here_doc_str(input); //WILL NEED TO FEED A STRUCT WITH ALL the Variables needed;
		if (write(fd, input, ft_strlen(input)) == -1)
            perror_exit(*gc, errno, WRITE_ERR_MSG);
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

// char *expand_here_doc_str(t_env_node *env, t_garbage_collect **gc, char *array, char *error_value)
// {
// 	int	i;
// 	int size;
// 	int total_size;
// 	char *expanded_var;
// 	char *tmp;
// 	i = 0;
// 	if (array == NULL)
// 		return (NULL);

// 	total_size = count_new_size_of_array(array, env, gc, error_value);
// 	expanded_var = malloc_trash(total_size + 1, gc);
// 	expanded_var[total_size] = '\0';
// 	size = 0;
// 	while (array[i])
// 	{
// 		if (array[i] == '$')
// 		{
// 			tmp = setter_gc(create_string_to_expand(&(array[i + 1]), gc), gc);
// 			if (ft_strlen(tmp) == 0)
// 			{	
// 				expanded_var[size++] = '$';
// 				i++;
// 			}
// 			else
// 			{
// 				i += ft_strlen(tmp) + 1;
// 				if (tmp && tmp[0] == '?')
// 					tmp = error_value;
// 				else
// 					tmp = setter_gc(get_env_variable(env, tmp), gc);
// 				while (tmp && *tmp && size < total_size)
// 				{	
// 					expanded_var[size++] = *tmp;
// 					tmp++;
// 				}
// 			}
// 		}
// 		else	
// 			expanded_var[size++] = array[i++];
// 	}
// 	return (expanded_var);
// }