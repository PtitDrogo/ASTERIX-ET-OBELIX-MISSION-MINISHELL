/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parsing_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 16:59:55 by tfreydie          #+#    #+#             */
/*   Updated: 2024/02/03 18:46:43 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

char	***ft_arg_parsing(char *argv[], t_cmd *cmd_line)
{
	int		i;
	char	***list_of_commands;
	char	*raw_command;
	char	*scriptprefix;

	scriptprefix = "/bin/bash -c ";
	list_of_commands = malloc(sizeof(char **) * (cmd_line->command_number + 1));
	if (!list_of_commands)
		return (NULL);
	i = -1;
	while (++i < cmd_line->command_number)
	{
		raw_command = argv[i + 2 + cmd_line->here_doc];
		if (raw_command[0] == '.' && raw_command[1] == '/')
		{
			raw_command = ft_strjoin(scriptprefix, raw_command);
			list_of_commands[i] = ft_split(raw_command, ' ');
			free(raw_command);
		}
		else
			list_of_commands[i] = ft_split(raw_command, ' ');
	}
	list_of_commands[i] = NULL;
	return (list_of_commands);
}
