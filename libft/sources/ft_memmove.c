/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 14:12:05 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/14 13:16:04 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
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
		return (dest);
	}
	else
	{
		ft_memcpy(dest, src, n);
		return (dest);
	}
}
