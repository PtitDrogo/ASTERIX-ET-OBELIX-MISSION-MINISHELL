/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 18:11:38 by garivo            #+#    #+#             */
/*   Updated: 2024/06/11 19:00:21 by garivo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_quoted_words(char *input, size_t *i)
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
		if (!input[(*i)++])
			return (ft_printf2("Unclosed quote\n"), -1);
		return (1);
	}
	return (0);
}

size_t	count_unquoted_words(char *input, size_t *i,
	int *firstchar)
{
	size_t	wc;

	wc = 0;
	if (input[(*i)] == ' ')
	{
		while (input[(*i)] == ' ')
			(*i)++;
		*firstchar = 1;
	}
	else if ((input[(*i)] == '<' || input[(*i)] == '>'
			|| input[(*i)] == '|') && ++wc > 0)
	{
		if (input[(*i) + 1] == input[(*i)])
			(*i)++;
		(*i)++;
		*firstchar = 1;
	}
	else if (input[*i] && *firstchar && ++(*i) > 0)
	{
		wc++;
		*firstchar = 0;
	}
	else
		(*i)++;
	return (wc);
}

size_t	get_extract_len(char *input)
{
	char	separator;
	size_t	i;
	size_t	quote_start;
	size_t	len;

	i = 0;
	len = 0;
	while (input[i] && input[i] != ' ' && input[i] != '<' && input[i] != '>')
	{
		quote_start = i;
		separator = 0;
		if ((input[i] == '\'' || input[i] == '"') && ++i && ++len)
		{
			separator = input[i - 1];
			while (input[i] && input[i] != separator && ++len)
				i++;
			len++;
		}
		else if (++i > 0 && ++len > 0)
			;
	}
	return (len);
}

static char	**quote_splitting(char *input, char **res, t_gc **gc)
{
	size_t	i;
	size_t	j;
	size_t	start;
	size_t	len;

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
		extract(&input[i], res[j], &i);
		if (len > 0)
			j++;
		else if (start == i)
			i++;
	}
	res[j] = NULL;
	return (res);
}

char	**quote_split(char *input, t_gc **gc, t_data *data)
{
	char	**res;
	size_t	wc;

	wc = count_words(input, data);
	if (wc == 0)
		return (NULL);
	res = malloc_trash(((wc + 1) * sizeof(char *)), gc);
	return (quote_splitting(input, res, gc));
}
