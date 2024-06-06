/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 17:40:04 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 13:01:07 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static	char	*n_flag_logic(char *str, bool *n_flag);
static	char	*join_echo(char **to_echo, t_gc **gc);

int	echo(char **to_echo, t_gc **gc)
{
	bool	n_flag;
	bool	n_n_flag;
	char	*joined_string;

	if (to_echo == NULL)
		return (1);
	if (to_echo[1] == NULL)
	{
		if (printf("\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
		return (0);
	}
	joined_string = join_echo(&to_echo[1], gc);
	joined_string = n_flag_logic(joined_string, &n_n_flag);
	n_flag = n_n_flag;
	while (n_n_flag == true)
		joined_string = n_flag_logic(joined_string, &n_n_flag);
	if (printf("%s", joined_string) == -1)
		perror_exit(*gc, errno, WRITE_ERR_MSG);
	if (n_flag == false)
	{
		if (printf("\n") == -1)
			perror_exit(*gc, errno, WRITE_ERR_MSG);
	}
	return (0);
}

static	char	*join_echo(char **to_echo, t_gc **gc)
{
	int		letters;
	int		i;
	int		j;
	char	*str_to_return;

	if (to_echo == NULL)
		return (NULL);
	i = 0;
	letters = 0;
	while (to_echo[i])
		letters += ft_len(to_echo[i++]);
	letters += count_arrays_in_doubleptr((void **)to_echo) - 1;
	str_to_return = malloc_trash(letters + 1, gc);
	str_to_return[letters] = '\0';
	i = 0;
	letters = 0;
	while (to_echo[i])
	{
		j = 0;
		while (to_echo[i][j])
			str_to_return[letters++] = to_echo[i][j++];
		if (to_echo[++i])
			str_to_return[letters++] = ' ';
	}
	return (str_to_return);
}

static	char	*n_flag_logic(char *str, bool *n_flag)
{
	int	i;

	if (ft_len(str) < 2)
	{
		*n_flag = false;
		return (str);
	}
	if (str[0] != '-' && str[1] != 'n')
	{
		*n_flag = false;
		return (str);
	}
	i = 2;
	while (str[i] && str[i] != ' ')
	{
		if (str[i++] != 'n')
		{
			*n_flag = false;
			return (str);
		}
	}
	*n_flag = true;
	if (str[i] == ' ')
		i++;
	return (&str[i]);
}
