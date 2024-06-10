/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 13:17:34 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/07 18:39:57 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	j = 0;
	if (little[0] == '\0')
		return ((char *)big);
	while (big[j] != '\0' && j < len)
	{
		i = 0;
		if (big[j] == little[i])
		{
			while (big[i + j] == little[i] && big[i + j] && (j + i) < len)
			{
				i++;
			}
			if (little[i] == '\0')
			{	
				return ((char *)(big + j));
			}
		}
		j++;
	}
	return (NULL);
}
