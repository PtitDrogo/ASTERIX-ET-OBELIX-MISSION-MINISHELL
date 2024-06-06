/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 14:10:35 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/05 15:09:46 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*get_next_first_token(t_token *token_root)
{
	t_token	*current;

	current = token_root;
	while (current)
	{
		if (current->type == PIPE)
			return (current);
		current = current->next;
	}
	return (NULL);
}

char	*ft_strjoin_and_add(char const *s1, char const *s2, char c)
{
	char	*joined;
	size_t	i;
	size_t	j;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	j = 0;
	joined = malloc(sizeof(char) * (ft_len(s1) + ft_len(s2) + 2));
	if (!joined)
		return (NULL);
	while (i < ft_len(s1))
	{
		joined[i] = s1[i];
		i++;
	}
	joined[i] = c;
	i++;
	while (j < ft_len(s2))
	{
		joined[i++] = s2[j++];
	}
	joined[i] = '\0';
	return (joined);
}

void	close_all_pipes(int **pipes_fds, t_gc *gc, int number_of_pipes)
{
	int	i;

	i = 0;
	if (pipes_fds == NULL)
		return ;
	while (i < number_of_pipes)
	{
		if (close(pipes_fds[i][0]) == -1)
			perror_exit(gc, EXIT_FAILURE, "Failed to close pipe");
		if (close(pipes_fds[i][1]) == -1)
			perror_exit(gc, EXIT_FAILURE, "Failed to close pipe");
		i++;
	}
	return ;
}

void	close_all_heredoc_pipes(t_cmd *cmds_root, t_gc *gc)
{
	t_token	*current;

	while (cmds_root)
	{
		current = cmds_root->redirection_in;
		while (current)
		{
			if (current->type == D_LESS)
			{
				if (current->token_fd != -1)
					if (close(current->token_fd) == -1)
						perror_exit(gc, EXIT_FAILURE, "Failed to close pipe");
			}
			current = current->next;
		}
		cmds_root = cmds_root->next;
	}
	return ;
}

char	*try_path(char *path)
{
	if (access(path, X_OK) == 0)
		return (path);
	return (NULL);
}
