/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 19:01:49 by tfreydie          #+#    #+#             */
/*   Updated: 2024/02/03 18:41:51 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

size_t	gnl_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	*ft_memset(void *s, int c, size_t n)
{
	size_t	i;
	char	*copy;

	copy = (char *)s;
	i = 0;
	while (i < n)
	{
		copy[i] = c;
		i++;
	}
	return (s);
}

void	*ft_memmove(void *dest, void *src, int n)
{
	char	*from;
	char	*to;
	int		i;

	from = (char *)src;
	to = (char *)dest;
	if (from == to || n == 0)
		return (dest);
	else if (to > from)
	{
		i = n - 1;
		while (i >= 0)
		{
			to[i] = from[i];
			i--;
		}
	}
	else
	{
		i = -1;
		while (++i < n)
			to[i] = from[i];
	}
	return (dest);
}

void	*free_and_null(char *line)
{
	free(line);
	return (NULL);
}

char	*final_check(char *line)
{
	if (line[0])
		return (line);
	return (free_and_null(line));
}
