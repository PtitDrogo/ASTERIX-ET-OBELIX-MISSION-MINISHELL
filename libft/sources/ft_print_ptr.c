/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_ptr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 13:31:33 by tfreydie          #+#    #+#             */
/*   Updated: 2024/03/13 13:45:12 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_recursive_print(unsigned long long ptr, int *size);

void	ft_print_ptr(unsigned long long ptr, int *size)
{
	if (ptr == 0)
		ft_putncount_str("(nil)", size);
	else
	{
		ft_putncount_str("0x", size);
		ft_recursive_print(ptr, size);
	}
}

static void	ft_recursive_print(unsigned long long ptr, int *size)
{
	char	*base;

	base = "0123456789abcdef";
	if (ptr >= 16 && *size != -1)
	{
		ft_recursive_print(ptr / 16, size);
		ft_recursive_print(ptr % 16, size);
	}
	else if (ptr < 16 && *size != -1)
		ft_putncount_char(base[ptr], size);
}
