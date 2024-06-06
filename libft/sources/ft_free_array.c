/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_array.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 18:32:26 by tfreydie          #+#    #+#             */
/*   Updated: 2024/06/06 14:32:39 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_free_array(void **array)
{
	int	j;

	j = 0;
	if (!array)
		return ;
	while (array[j])
	{
		if (array[j])
		{
			free(array[j]);
			array[j] = NULL;
		}
		j++;
	}
	free(array);
	return ;
}
