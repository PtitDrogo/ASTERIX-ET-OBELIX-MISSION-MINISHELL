/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_array.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 18:32:26 by tfreydie          #+#    #+#             */
/*   Updated: 2024/03/13 13:58:59 by tfreydie         ###   ########.fr       */
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
			free(array[j]);
		j++;
	}
	free(array);
	return ;
}
