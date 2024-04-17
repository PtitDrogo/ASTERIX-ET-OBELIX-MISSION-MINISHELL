/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:39:00 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/10 16:05:34 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	j;
	size_t	src_len;
	size_t	dest_len;

	i = 0;
	j = 0;
	dest_len = ft_strlen(dest);
	src_len = ft_strlen(src);
	while (dest[i])
		i++;
	if (size < i || size == 0)
		return (size + src_len);
	while (src[j] != '\0' && i < size - 1)
	{
		dest[i] = src[j];
		j++;
		i++;
	}
	dest[i] = '\0';
	return (dest_len + src_len);
}
