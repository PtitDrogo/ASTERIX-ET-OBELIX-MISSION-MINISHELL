/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garivo <garivo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 18:11:38 by garivo            #+#    #+#             */
/*   Updated: 2024/04/27 04:42:21 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_quoted_words(char *input, size_t *i,
	int *firstchar, t_garbage_collect **gc)
{
	size_t	quote;
	char	separator;

	separator = 0;
	if (input[*i] == '"' || input[*i] == '\'')
		separator = input[*i];
	quote = *i;
	if (separator != 0)
	{
		while (input[++*i] && input[*i] != separator)
			*i = *i;
		if (!input[*i] || ++*i == -1)
			ft_error("Unclosed quote", *gc);
		*firstchar = 1;
		if (*i != quote + 2)
			return (1);
		else
			return (2);
	}
	return (0);
}

static size_t	count_words(char *input, t_garbage_collect **gc)
{
	size_t	i;
	size_t	wc;
	int		quoted_count;
	int		firstchar;

	wc = 0;
	firstchar = 1;
	i = 0;
	while (input[i])
	{
		quoted_count = count_quoted_words(input, &i, &firstchar, gc);
		if (quoted_count == 1 && firstchar && ++wc > 0 || quoted_count == 2)
			firstchar = 0;
		else if (input[i] == ' ')
		{
			while (input[i] == ' ')
				i++;
			firstchar = 1;
		}
		else if (firstchar && ++wc > 0 && ++i > 0)
			firstchar = 0;
		else
			i++;
	}
	return (wc);
}

static size_t	extract_len(char *input)
{
	char	separator;
	size_t	i;
	size_t	quote_start;
	size_t	len;

	i = 0;
	len = 0;
	while (input[i] && input[i] != ' ')
	{
		quote_start = i;
		separator = 0;
		if ((input[i] == '\'' || input[i] == '"') && ++i && ++len)
		{
			separator = input[i - 1];
			while (input[i] && input[i] != separator && ++len)
				i++;
			if (input[i] && i++ - quote_start == 1)
				len -= 1;
			else
				len++;
		}
		else
		{
			i++;
			len++;
		}
	}

	/*separator = ' ';
	if (*input == '\'' || *input == '"')
	{
		separator = *input;
		i++;
	}
	while (input[i] && input[i] != separator)
		i++;
	if (separator != ' ' && i == 1 && ++*index > 0)
		return (0);
	if (separator != ' ' && i > 0)
		i++;*/
	return (len);
}

char	*ft_strncat(char *src, char *dst, int len)
{
	int	i;
	int	size;

	i = 0;
	//printf("cat : %s\n", dst);
	size = ft_strlen(dst);
	while (src[i] && i < len)
	{
		dst[size + i] = src[i];
		i++;
	}
	dst[size + i] = '\0';
	return (dst);
}

static char	*extract(char *input, char *res, size_t *index)
{
	char	separator;
	size_t	i;
	size_t	quote_start;

	i = 0;
	while (input[i] && input[i] != ' ')
	{
		quote_start = i;
		separator = 0;
		if ((input[i] == '\'' || input[i] == '"') && ++i && ++*index)
		{
			separator = input[i - 1];
			while (input[i] && input[i] != separator && ++*index)
				i++;
			if (i - quote_start > 1)
				ft_strncat(&input[quote_start], res, i - quote_start + 1);
		}
		else
			ft_strncat(&input[i], res, 1);
		//printf("Word : %s\n", res);
		i++;
		(*index)++;
	}
	return (res);
}

char	**quote_split(char *input, t_garbage_collect **gc)
{
	char	**res;
	size_t	i;
	size_t	j;
	size_t	len;
	size_t	start;

	res = malloc_trash((count_words(input, gc) + 1) * sizeof(char *), gc);
	i = 0;
	j = 0;
	while (input[i])
	{
		start = i;
		len = extract_len(&input[i]);
		if (len > 0)
		{
			res[j] = malloc_trash(len + 1, gc);
			*res[j] = '\0';
		}
		res[j] = extract(&input[i], res[j], &i);
		if (len > 0)
			j++;
		else if (start == i)
			i++;
	}
	res[j] = NULL;
	return (res);
}
