/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_err_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 15:31:36 by tfreydie          #+#    #+#             */
/*   Updated: 2024/04/23 15:45:29 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void			ft_recursive_print(unsigned long long ptr, int *size);
static unsigned int	ft_base_length(char *base);

void	ft_putnbr_err(int nbr, int *size)
{
	long long	real_nbr;
	int			digit;

	real_nbr = nbr;
	if (real_nbr < 0 && *size != -1)
	{
		real_nbr *= -1;
		ft_putncount_char_err('-', size);
	}
	if (real_nbr >= 10 && *size != -1)
	{
		ft_putnbr_err(real_nbr / 10, size);
		ft_putnbr_err(real_nbr % 10, size);
	}
	else if (real_nbr < 10 && *size != -1)
	{
		digit = real_nbr + '0';
		ft_putncount_char_err(digit, size);
	}
}

void	ft_print_ptr_err(unsigned long long ptr, int *size)
{
	if (ptr == 0)
		ft_putncount_str_err("(nil)", size);
	else
	{
		ft_putncount_str_err("0x", size);
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
		ft_putncount_char_err(base[ptr], size);
}

void	ft_putuns_err(unsigned int nbr, char *base, int *size)
{
	unsigned int		base_len;

	base_len = ft_base_length(base);
	if (nbr >= base_len && *size != -1)
	{
		ft_putuns_err(nbr / base_len, base, size);
		ft_putuns_err(nbr % base_len, base, size);
	}
	else if (nbr < base_len && *size != -1)
		ft_putncount_char_err(base[nbr], size);
}

static unsigned int	ft_base_length(char *base)
{
	int	i;

	i = 0;
	while (base[i] != '\0')
		i++;
	return (i);
}
