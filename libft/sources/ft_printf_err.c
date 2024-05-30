/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_err.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 17:09:05 by tfreydie          #+#    #+#             */
/*   Updated: 2024/05/30 02:59:40 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_strlen_printf_err(const char *s);

int	ft_printf_err(const char *text, ...)
{
	va_list	argptr;
	size_t	i;
	int		actual_size;

	va_start(argptr, text);
	actual_size = 0;
	i = 0;
	if (!text || write(2, NULL, 0) == -1)
		actual_size = -1;
	while (text[i] && actual_size != -1)
	{
		if (text[i] == '%')
		{
			i++;
			ft_special_char_err(text[i], argptr, &actual_size);
		}
		else
			ft_putncount_char_err(text[i], &actual_size);
		i++;
	}
	va_end(argptr);
	return (actual_size);
}

void	ft_special_char_err(char c, va_list argptr, int *size)
{
	if (c == 'c')
		ft_putncount_char_err(va_arg(argptr, int), size);
	else if (c == 's')
		ft_putncount_str_err(va_arg(argptr, char *), size);
	else if (c == 'p')
		ft_print_ptr_err(va_arg(argptr, unsigned long long), size);
	else if (c == 'd')
		ft_putnbr_err(va_arg(argptr, int), size);
	else if (c == 'i')
		ft_putnbr_err(va_arg(argptr, int), size);
	else if (c == 'u')
		ft_putuns_err(va_arg(argptr, unsigned int), "0123456789", size);
	else if (c == 'x')
		ft_putuns_err(va_arg(argptr, unsigned int), "0123456789abcdef", size);
	else if (c == 'X')
		ft_putuns_err(va_arg(argptr, unsigned int), "0123456789ABCDEF", size);
	else if (c == '%')
		ft_putncount_char_err('%', size);
	else if (c == '\0')
		*size = -1;
	else
	{
		ft_putncount_char_err('%', size);
		ft_putncount_char_err(c, size);
	}
}

void	ft_putncount_char_err(char c, int *size)
{
	if (write(2, &c, 1) == -1)
		*size = -1;
	else
		*size += 1;
}

void	ft_putncount_str_err(char *str, int *size)
{
	int	strlen;

	strlen = ft_strlen_printf_err(str);
	if (!str)
		ft_putncount_str("(null)", size);
	else if (write(2, str, strlen) == -1)
		*size = -1;
	else
		*size += strlen;
}

static size_t	ft_strlen_printf_err(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}
