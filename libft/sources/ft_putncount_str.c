/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putncount_str.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 20:16:06 by tfreydie          #+#    #+#             */
/*   Updated: 2024/03/13 13:44:39 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_strlen_printf(const char *s);

void	ft_putncount_str(char *str, int *size)
{
	int	strlen;

	strlen = ft_strlen_printf(str);
	if (!str)
		ft_putncount_str("(null)", size);
	else if (write(1, str, strlen) == -1)
		*size = -1;
	else
		*size += strlen;
}

static size_t	ft_strlen_printf(const char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}
