/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ptitdrogo <ptitdrogo@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:47:43 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/10 13:18:40 by ptitdrogo        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//TODO, somehow get the initial call to heredoc to the history
static char		*readline_n_add_n(char *readline, t_garbage_collect **gc);
static int		ft_strncmp_n(char *input, char *delimiter, size_t n);

//here_doc that will write into the fd we give it, it doesnt update history because life is hard.
int	here_doc(char *delimiter, t_garbage_collect **gc, int fd)
{
	char	*input;

    while (1)
	{
		input = readline_n_add_n(readline("heredoc> "), gc);
		if (input == NULL)
			if (ft_printf_err("bash: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter) == -1)
				return (0); //CTRL D doesnt exit shell just the current heredoc; (if its the last here_doc it exits the shell tho);
        if (ft_strncmp_n(input, delimiter, ft_strlen(input)) == 0)
		{	
			// printf("I am breaking out of heredoc loop'n");
			break ;
		}
		if (write(fd, input, ft_strlen(input)) == -1)
            perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	printf("Returning 1 in heredoc\n");
	return (1);
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
	printf("cmp is about to return %i\n",(unsigned char)input[i] - (unsigned char)delimiter[i] );
    return ((unsigned char)input[i] - (unsigned char)delimiter[i]);
}

