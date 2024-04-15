/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 14:23:35 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/23 17:40:13 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t	i;
	char	*big;

	big = (char *)s;
	i = 0;
	while (i < n)
	{
		if (big[i] == (char)c)
			return ((void *)big + i);
		i++;
	}
	return (NULL);
}
