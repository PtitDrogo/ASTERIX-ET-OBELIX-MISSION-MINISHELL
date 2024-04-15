/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 20:15:44 by tfreydie          #+#    #+#             */
/*   Updated: 2023/12/07 17:56:16 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr(int nbr, int *size)
{
	long long	real_nbr;
	int			digit;

	real_nbr = nbr;
	if (real_nbr < 0 && *size != -1)
	{
		real_nbr *= -1;
		ft_putncount_char('-', size);
	}
	if (real_nbr >= 10 && *size != -1)
	{
		ft_putnbr(real_nbr / 10, size);
		ft_putnbr(real_nbr % 10, size);
	}
	else if (real_nbr < 10 && *size != -1)
	{
		digit = real_nbr + '0';
		ft_putncount_char(digit, size);
	}
}
