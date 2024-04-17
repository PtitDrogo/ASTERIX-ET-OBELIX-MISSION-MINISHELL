/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 16:15:16 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/09 11:01:13 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *src)
{
	int		length;
	int		i;
	char	*dest;

	i = 0;
	length = 0;
	while (src[length] != '\0')
	{
		length++;
	}
	dest = malloc(sizeof(char) * length + 1);
	if (!dest)
		return (NULL);
	while (i < length)
	{
		dest[i] = src[i];
		i++;
	}
	dest[length] = '\0';
	return (dest);
}
