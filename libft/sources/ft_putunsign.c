/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putunsign.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 15:04:32 by tfreydie          #+#    #+#             */
/*   Updated: 2024/03/13 13:42:07 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static unsigned int	ft_base_length(char *base);

void	ft_putunsign(unsigned int nbr, char *base, int *size)
{
	unsigned int		base_len;

	base_len = ft_base_length(base);
	if (nbr >= base_len && *size != -1)
	{
		ft_putunsign(nbr / base_len, base, size);
		ft_putunsign(nbr % base_len, base, size);
	}
	else if (nbr < base_len && *size != -1)
		ft_putncount_char(base[nbr], size);
}

static unsigned int	ft_base_length(char *base)
{
	int	i;

	i = 0;
	while (base[i] != '\0')
		i++;
	return (i);
}
