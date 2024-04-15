/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 16:10:15 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/23 14:53:55 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total_len;
	void	*ptr;

	total_len = nmemb * size;
	if (!nmemb || !size)
	{
		ptr = malloc(0);
		if (!ptr)
			return (NULL);
		return (ptr);
	}
	if (total_len / nmemb != size)
		return (NULL);
	ptr = malloc(total_len * sizeof(char));
	if (!ptr)
		return (NULL);
	ft_memset(ptr, '\0', total_len);
	return (ptr);
}
