/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_parsing_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 18:03:18 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/27 03:19:42 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

static char	*ft_strjoin_and_add(char const *s1, char const *s2, char c);

char	*find_valid_path(t_cmd *cmd)
{
	int		i;
	char	*current_path;

	i = 0;
	current_path = ft_strdup_secure(cmd->commands[cmd->current_process][0]);
	if (!current_path)
		free_all_and_exit(cmd, "failed to create valid path");
	else if (access(current_path, X_OK) == 0)
		return (current_path);
	else
		free(current_path);
	while (cmd->possible_paths[i])
	{
		current_path = ft_strjoin_and_add(cmd->possible_paths[i],
				cmd->commands[cmd->current_process][0], '/');
		if (!current_path)
			free_all_and_exit(cmd, "failed to create valid path");
		if (access(current_path, X_OK) == 0)
			return (current_path);
		free(current_path);
		i++;
	}
	return (NULL);
}

char	*find_env_variable(char **envp, char *env_to_find)
{
	int	i;
	int	len_env;

	if (!env_to_find || !envp || !envp[0]) // uh ?
	{
		perror("Environnement does not exist");
		exit(EXIT_FAILURE);
	}
	len_env = ft_strlen(env_to_find);
	i = 0;
	while (envp[i])
	{
		if (ft_strnstr(envp[i], env_to_find, len_env))
			return (envp[i] + len_env); // this is so i just get the content
		i++;
	}
	return (NULL);
}

static char	*ft_strjoin_and_add(char const *s1, char const *s2, char c)
{
	char	*joined;
	size_t	i;
	size_t	j;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	j = 0;
	joined = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 2));
	if (!joined)
		return (NULL);
	while (i < ft_strlen(s1))
	{
		joined[i] = s1[i];
		i++;
	}
	joined[i] = c;
	i++;
	while (j < ft_strlen(s2))
	{
		joined[i++] = s2[j++];
	}
	joined[i] = '\0';
	return (joined);
}
