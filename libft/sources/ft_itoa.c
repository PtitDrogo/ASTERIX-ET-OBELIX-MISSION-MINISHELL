/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:44:14 by tfreydie          #+#    #+#             */
/*   Updated: 2023/11/21 13:31:28 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	num_digits(int n);

char	*ft_itoa(int n)
{
	int		count;
	int		is_negative;
	char	*array;

	is_negative = (n < 0);
	count = num_digits(n);
	array = malloc(sizeof(char) * (count + 1));
	if (!array)
		return (NULL);
	array[count--] = '\0';
	if (is_negative)
		array[0] = '-';
	while (count >= (0 + is_negative))
	{
		array[count--] = ((n % 10) * ((n > 0) - (n < 0))) + '0';
		n /= 10;
	}
	return (array);
}

static	int	num_digits(int n)
{
	int	temp;
	int	count;

	count = 0;
	temp = n;
	if (temp == 0)
		count = 1;
	else
	{
		while (temp != 0)
		{
			count++;
			temp /= 10;
		}
	}
	count += (n < 0);
	return (count);
}
