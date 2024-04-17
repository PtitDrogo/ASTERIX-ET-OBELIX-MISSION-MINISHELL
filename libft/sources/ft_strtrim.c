/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 12:32:44 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/23 19:54:25 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_set(char c, const char *set);

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	int		start;
	int		end;
	char	*array;

	i = 0;
	start = 0;
	end = ft_strlen(s1) - 1;
	if (set[0])
	{
		while (s1[start] && is_set(s1[start], set) && start <= end)
			start++;
		while (end >= 0 && is_set(s1[end], set) && start <= end)
			end--;
	}
	array = malloc(sizeof(char) * (end - start + 2));
	if (!array)
		return (NULL);
	while (start <= end)
		array[i++] = s1[start++];
	array[i] = '\0';
	return (array);
}

static int	is_set(char c, const char *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (c == set[i])
			return (1);
		i++;
	}
	return (0);
}
